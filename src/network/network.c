#include <err.h>
#include <malloc.h>
#include <stdio.h>
#include "network.h"
#include "function.h"

/// checks if a pointer is null.
#define CHK(ptr, function, message)               \
    {                                             \
        if (ptr == NULL)                          \
            errx(1, "%s: %s", function, message); \
    }

matrix_t *compute_layer(
    matrix_t *values, layer_t *layer, mat_transform_t activation)
{
    matrix_t *result = matrix_create(layer->count, 1, 0);
    for (size_t i = 0; i < layer->count; i++)
    {
        neuron_t *neuron = layer->neurons[i];
        matrix_t *tmp = mat_product(neuron->weights, values);
        double postres = mat_el_at(tmp, 0, 0) + neuron->bias;
        postres = activation(postres);
        matrix_free(tmp);
        mat_set_el(result, i, 0, postres);
    }

    return result;
}

matrix_t *compute_result(matrix_t *values, network_t *network)
{
    matrix_t *lastResults = mat_copy(values);
    size_t i;
    for (i = 0; i < network->layer_count - 1; i++)
    {
        layer_t *layer = network->layers[i];
        matrix_t *tmp = compute_layer(lastResults, layer, activation);
        matrix_free(lastResults);
        lastResults = tmp;
    }

    layer_t *layer = network->layers[i];
    matrix_t *tmp = compute_layer(lastResults, layer, output_activation);
    matrix_free(lastResults);
    lastResults = tmp;

    return lastResults;
}

void compute_result_with_save(matrix_t **values, network_t *network)
{
    size_t i;
    for (i = 0; i < network->layer_count - 1; i++)
    {
        layer_t *layer = network->layers[i];
        values[i + 1] = compute_layer(values[i], layer, activation);
    }

    layer_t *layer = network->layers[i];
    values[i + 1] = compute_layer(values[i], layer, output_activation);
}

neuron_t *neuron_create(matrix_t *weights, double bias)
{
    neuron_t *neuron = (neuron_t *)malloc(sizeof(neuron_t));
    if (neuron == NULL)
        return NULL;
    neuron->weights = weights;
    neuron->bias = bias;

    return neuron;
}

void neuron_free(neuron_t *neuron)
{
    matrix_free(neuron->weights);
    free(neuron);
}

layer_t *layer_create_from_mat(
    matrix_t **weights, double *biases, size_t count)
{
    layer_t *layer = (layer_t *)malloc(sizeof(layer_t));
    if (layer == NULL)
        return NULL;
    layer->count = count;
    layer->neurons = (neuron_t **)malloc(count * sizeof(neuron_t *));
    if (layer->neurons == NULL)
        return NULL;
    for (size_t i = 0; i < count; i++)
        layer->neurons[i] = neuron_create(weights[i], biases[i]);

    return layer;
}

layer_t *layer_create(neuron_t **neurons, size_t count)
{
    layer_t *layer = (layer_t *)malloc(sizeof(layer_t));
    if (layer == NULL)
        return NULL;
    layer->neurons = neurons;
    layer->count = count;

    return layer;
}

void layer_free(layer_t *layer)
{
    for (size_t i = 0; i < layer->count; i++)
    {
        neuron_free(layer->neurons[i]);
    }

    free(layer->neurons);
    free(layer);
}

network_t *network_create(layer_t **layers, size_t layer_count, size_t inputs)
{
    network_t *res = (network_t *)malloc(sizeof(network_t));
    if (res == NULL)
        return NULL;
    res->layers = layers;
    res->inputs = inputs;
    res->layer_count = layer_count;
    return res;
}

void network_free(network_t *net)
{
    for (size_t i = 0; i < net->layer_count; i++)
    {
        layer_free(net->layers[i]);
    }

    free(net->layers);
    free(net);
}

#define READ_LENGTH 1024

static inline size_t parseSize(size_t old, char c)
{
    return old * 10 + (c - '0');
}

static double parseNumber(double old, char c, size_t *offset, int negative)
{
    if (c == '.')
    {
        *offset = 1;
        return old;
    }

    double value = c - '0';
    if (*offset > 0)
    {
        for (size_t i = 0; i < *offset; i++)
        {
            value /= 10.0;
        }
        (*offset)++;
        if (negative)
            old -= value;
        else
            old += value;
    }
    else
    {
        if (negative)
            old = old * 10 - value;
        else
            old = old * 10 + value;
    }

    return old;
}

inline static void parser_error(size_t line, size_t column, char c)
{
    errx(1, "unknown character \'%c\'(%d) while parsing at position (%zu,%zu)",
        c, c, line, column);
}

inline static int parse_header(
    FILE *file, size_t *layer_count, size_t *input_neurons, size_t *line)
{

    char buffer[READ_LENGTH];
    size_t comma_count = 0;
    size_t value = 0;
    size_t index = 0;
    int in_number = 0;
    while (1)
    {
        if (fgets(buffer, READ_LENGTH, file) == NULL)
            return 0;
        for (size_t i = 0; i < READ_LENGTH; i++, index++)
        {
            char c = buffer[i];
            if (c >= '0' && c <= '9')
            {
                in_number = 1;
                value = parseSize(value, c);
            }
            else if (c == ',')
            {
                if (comma_count == 0)
                {
                    *layer_count = value;
                    value = 0;
                }
                else
                {
                    parser_error(0, index, c);
                }
                comma_count++;
            }
            else if (c == '\n')
            {
                // only encountered new line
                if (comma_count == 0 && !in_number)
                {
                    (*line)++;
                    break;
                }
                else if (comma_count == 1)
                    *input_neurons = value;
                else
                    parser_error(0, index, c);
                return 1;
            }
            else if (c == ' ')
            {
                in_number = 0;
            }
            else
            {
                parser_error(*line, index, c);
            }
        }
    }

    return 0;
}

inline static int parse_layer_header(
    FILE *file, size_t *neuron_count, size_t *line)
{

    char buffer[READ_LENGTH];

    size_t value = 0;
    int in_number = 0;
    size_t index = 0;
    while (1)
    {
        if (fgets(buffer, READ_LENGTH, file) == NULL)
            return 0;
        for (size_t i = 0; i < READ_LENGTH; i++, index++)
        {
            char c = buffer[i];
            if (c >= '0' && c <= '9')
            {
                in_number = 1;
                value = parseSize(value, c);
            }
            else if (c == '\n')
            {
                if (!in_number)
                {
                    (*line)++;
                    index = 0;
                    break;
                }
                *neuron_count = value;
                return 1;
            }
            else if (c == '\0')
            {
                *neuron_count = value;
                return 0;
            }
            else if (c != ' ')
            {
                parser_error(*line, index, c);
            }
        }
    }

    return 0;
}

inline static int parse_neuron_line(
    FILE *file, double *result, size_t count, size_t *line)
{

    char buffer[READ_LENGTH];
    /// the col index for errors
    size_t col = 0;
    size_t index = 0;
    size_t value_offset = 0;
    double value = 0;
    int value_negative = 0;
    int in_number = 0;
    while (1)
    {
        if (fgets(buffer, READ_LENGTH, file) == NULL)
            return 0;
        for (size_t i = 0; i < READ_LENGTH; i++, col++)
        {
            char c = buffer[i];
            if (c == '-')
            {
                if (in_number)
                    parser_error(*line, col, c);
                value_negative = 1;
            }
            else if (c == '.')
            {
                if (value_offset > 0)
                    parser_error(*line, col, c);
                value = parseNumber(value, c, &value_offset, value_negative);
                in_number = 1;
            }
            else if ((c >= '0' && c <= '9'))
            {
                in_number = 1;
                value = parseNumber(value, c, &value_offset, value_negative);
            }
            else if (c == ',')
            {

                if (!in_number || index + 1 >= count)
                    parser_error(*line, col, c);
                in_number = 0;
                result[index++] = value;
                value = 0;
                value_negative = 0;
                value_offset = 0;
            }
            else if (c == '\0')
            {
                // result[index++] = value;
                if (index < count)
                {
                    break;
                    // errx(1, "missing values at line %zu", *line);
                }
                else
                {
                    return 0;
                }
            }
            else if (c == '\n')
            {
                if (!in_number && index == 0)
                {
                    col = 0;
                    (*line)++;
                    break;
                }
                result[index++] = value;
                if (index < count)
                    errx(1, "missing values at line %zu", *line);
                return 1;
            }
            else if (c != ' ')
            {
                parser_error(*line, col, c);
            }
        }
    }

    return 0;
}

static inline neuron_t *parse_results(double *values, size_t weight_count)
{
    matrix_t *weights = mat_create_fill(1, weight_count, values);
    return neuron_create(weights, values[weight_count]);
}

network_t *network_load(const char *path)
{
    // open the file
    FILE *file = fopen(path, "r");
    CHK(file, "network_load", "could not open file");

    size_t layer_count, weight_count, neuron_count = 0;
    size_t neuron_index = 0, layer_index = 0;
    int in_layer = 0;
    size_t line = 0;
    int alive = parse_header(file, &layer_count, &neuron_count, &line);
    line++;
    if (!alive)
        return NULL;

    layer_t **layers = (layer_t **)malloc(layer_count * sizeof(layer_t *));
    network_t *res = network_create(layers, layer_count, neuron_count);
    double *values;
    while (layer_index < layer_count)
    {
        if (!in_layer)
        {
            weight_count = neuron_count;
            alive = parse_layer_header(file, &neuron_count, &line);
            line++;
            neuron_index = 0;
            in_layer = 1;
            layers[layer_index] = layer_create(
                (neuron_t **)malloc(neuron_count * sizeof(neuron_t *)),
                neuron_count);
        }
        else
        {
            if (neuron_count == neuron_index)
            {
                in_layer = 0;
                layer_index++;
            }
            else
            {
                values = (double *)malloc((weight_count + 1) * sizeof(double));
                alive
                    = parse_neuron_line(file, values, weight_count + 1, &line);
                line++;
                layers[layer_index]->neurons[neuron_index]
                    = parse_results(values, weight_count);
                free(values);
                neuron_index++;
            }
        }
    }
    // close the file
    fclose(file);
    return res;
}

static inline void neuron_save(
    FILE *file, neuron_t *neuron, size_t weight_count)
{
    for (size_t i = 0; i < weight_count; i++)
    {
        fprintf(file, "%f, ", mat_el_at(neuron->weights, 0, i));
    }
    fprintf(file, "%f\n", neuron->bias);
}

static inline void layer_save(FILE *file, layer_t *layer, size_t weight_count)
{

    fprintf(file, "%zu\n", layer->count);
    for (size_t i = 0; i < layer->count; i++)
    {
        neuron_save(file, layer->neurons[i], weight_count);
    }
}

void network_save(const char *path, network_t *net)
{
    FILE *file = fopen(path, "w");
    CHK(file, "network_save", "could not open file");
    fprintf(file, "%zu, %zu\n", net->layer_count, net->inputs);
    size_t weight_count = net->inputs;
    for (size_t i = 0; i < net->layer_count; i++)
    {
        fprintf(file, "\n");
        layer_save(file, net->layers[i], weight_count);
        weight_count = net->layers[i]->count;
    }
    fclose(file);
}

network_t *network_generate(size_t *neuron_count, size_t size)
{
    if (size < 2)
        return NULL;
    layer_t **layers = (layer_t **)malloc((size - 1) * sizeof(layer_t *));
    size_t weights = neuron_count[0];
    network_t *res = network_create(layers, size - 1, weights);

    for (size_t i = 1; i < size; i++)
    {
        size_t n_count = neuron_count[i];
        neuron_t **neurons = (neuron_t **)malloc(n_count * sizeof(neuron_t *));
        layer_t *layer = layer_create(neurons, n_count);
        layers[i - 1] = layer;
        for (size_t ni = 0; ni < n_count; ni++)
        {
            double *values = generate_weights(weights + 1);
            layers[i - 1]->neurons[ni] = parse_results(values, weights);
            free(values);
        }
        weights = n_count;
    }

    return res;
}

static neuron_t *neuron_copy(neuron_t *old)
{
    return neuron_create(mat_copy(old->weights), old->bias);
}

static layer_t *layer_copy(layer_t *layer)
{
    neuron_t **neurons
        = (neuron_t **)malloc(layer->count * sizeof(neuron_t *));
    for (size_t i = 0; i < layer->count; i++)
    {
        neurons[i] = neuron_copy(layer->neurons[i]);
    }

    return layer_create(neurons, layer->count);
}

network_t *network_copy(network_t *old)
{
    layer_t **layers
        = (layer_t **)malloc(old->layer_count * sizeof(layer_t *));
    network_t *res = network_create(layers, old->layer_count, old->inputs);
    for (size_t i = 0; i < old->layer_count; i++)
    {
        layers[i] = layer_copy(old->layers[i]);
    }

    return res;
}

static double output_neuron_train(neuron_t *trained, neuron_t *old,
    double rate, double target, double output, matrix_t *previous_result)
{
    double delta = cost_derivative(target, output)
                   * output_activation_derivative(output);

    trained->bias -= rate * delta;
    for (size_t i = 0; i < trained->weights->columns; i++)
    {
        double value = mat_el_at(old->weights, 0, i)
                       - rate * delta * mat_el_at(previous_result, i, 0);
        mat_set_el(trained->weights, 0, i, value);
    }

    return delta;
}

static void compute_deltas(
    matrix_t **outputs, network_t *old, matrix_t **deltas)
{

    for (int i = old->layer_count - 2; i >= 0; i--)
    {
        layer_t *layer = old->layers[i];
        layer_t *prev_layer = old->layers[i + 1];
        matrix_t *delta = matrix_create(layer->count, 1, 0);
        deltas[i] = delta;
        for (size_t j = 0; j < layer->count; j++)
        {
            double tmp_delta = 0;
            for (size_t k = 0; k < prev_layer->count; k++)
            {
                neuron_t *prev_neuron = prev_layer->neurons[k];
                tmp_delta += mat_el_at(deltas[i + 1], k, 0)
                             * mat_el_at(prev_neuron->weights, 0, j)
                             * activation_derivative(
                                 mat_el_at(outputs[i + 1], j, 0));
            }
            mat_set_el(delta, j, 0, tmp_delta);
        }
    }
}

static void hidden_layer_train(
    matrix_t **outputs, network_t *new, matrix_t **deltas, double rate)
{

    for (int i = new->layer_count - 2; i >= 0; i--)
    {
        layer_t *layer = new->layers[i];
        for (size_t j = 0; j < layer->count; j++)
        {
            neuron_t *new_neuron = new->layers[i]->neurons[j];
            new_neuron->bias -= rate * mat_el_at(deltas[i], j, 0);
            for (size_t w = 0; w < new_neuron->weights->columns; w++)
            {
                double new_weight = mat_el_at(new_neuron->weights, 0, w);
                mat_set_el(new_neuron->weights, 0, w,
                    new_weight
                        - rate * mat_el_at(deltas[i], j, 0)
                              * mat_el_at(outputs[i], w, 0));
            }
        }
    }
}

void network_train(
    network_t **pnet, matrix_t *inputs, matrix_t *target, double rate)
{
    network_t *trained = network_copy(*pnet);

    matrix_t **outputs
        = (matrix_t **)malloc(((*pnet)->layer_count + 1) * sizeof(matrix_t *));
    outputs[0] = inputs;

    matrix_t **deltas
        = (matrix_t **)malloc((*pnet)->layer_count * sizeof(matrix_t *));

    compute_result_with_save(outputs, *pnet);

    size_t layer_i = trained->layer_count - 1;
    layer_t *trained_layer = trained->layers[layer_i];
    layer_t *old_layer = (*pnet)->layers[layer_i];
    deltas[layer_i] = matrix_create(trained_layer->count, 1, 0);
    for (size_t ni = 0; ni < trained_layer->count; ni++)
    {
        double delta = output_neuron_train(trained_layer->neurons[ni],
            old_layer->neurons[ni], rate, mat_el_at(target, ni, 0),
            mat_el_at(outputs[layer_i + 1], ni, 0), outputs[layer_i]);
        mat_set_el(deltas[layer_i], ni, 0, delta);
    }

    compute_deltas(outputs, *pnet, deltas);
    hidden_layer_train(outputs, trained, deltas, rate);

    for (size_t i = 1; i <= trained->layer_count; i++)
    {
        matrix_free(outputs[i]);
        matrix_free(deltas[i - 1]);
    }
    free(outputs);
    free(deltas);
    network_free(*pnet);
    *pnet = trained;
}

int network_get_output(matrix_t *outputs, double threshold)
{
    int result = -1;
    double old_value = 0;
    for (size_t i = 0; i < outputs->rows; i++)
    {
        double val = mat_el_at(outputs, i, 0);
        if (val < threshold)
            continue;
        if (val > old_value)
        {
            result = i;
            old_value = val;
        }
    }

    return result;
}
