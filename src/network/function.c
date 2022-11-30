#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "function.h"

#define UNUSED(x) (void)(x)

static double sigmoid(
    size_t layer, size_t neuron, double x, network_results_t *res)
{
    UNUSED(layer);
    UNUSED(neuron);
    UNUSED(res);
    return 1 / (1 + exp(-x));
}

static double sigmoid_derivative(
    size_t layer, size_t neuron, double x, network_results_t *res)
{

    UNUSED(layer);
    UNUSED(neuron);
    UNUSED(res);
    return x * (1 - x);
}

static double soft_max(
    size_t layeri, size_t neuron, double arg, network_results_t *res)
{
    UNUSED(arg);
    layer_t *layer = res->network->layers[layeri];
    double sum = 0, value = 0;
    double *cache;
    if (neuron == 0)
    {
        cache = (double *)calloc(layer->count + 1, sizeof(double));
        for (size_t i = 0; i < layer->count; i++)
        {
            double e = exp(mat_el_at(res->preactivation[layeri], i, 0));
            cache[i + 1] = e;
            sum += e;
        }
        cache[0] = sum;
        res->cache = cache;
        value = cache[neuron + 1];
    }
    else
    {
        cache = (double *)res->cache;
        value = cache[neuron + 1];
        sum = cache[0];
    }

    if (neuron == layer->count - 1)
    {
        free(res->cache);
        res->cache = NULL;
    }
    return value / sum;
}

static double soft_max_derivative(
    size_t layer, size_t neuron, double arg, network_results_t *res)
{
    UNUSED(layer);
    UNUSED(neuron);
    UNUSED(res);
    return arg * (1 - arg);
}

layer_activation_t activations[] = {{sigmoid, sigmoid_derivative, "sigmoid"},
    {soft_max, soft_max_derivative, "softmax"}};

layer_activation_t get_layer_activation(const char *name)
{
    if (name == NULL)
        return activations[0];
    size_t len = sizeof(activations) / sizeof(layer_activation_t);
    for (size_t i = 0; i < len; i++)
    {
        if (strcmp(activations[i].name, name) == 0)
        {
            return activations[i];
        }
    }
    return activations[0];
}

double cost(matrix_t *target, matrix_t *output)
{
    double res = 0;
    for (size_t i = 0; i < target->rows; i++)
    {
        res += pow(mat_el_at(target, i, 0) - mat_el_at(output, i, 0), 2);
    }

    return res / 2;
}

double cost_derivative(double target, double output)
{
    return output - target;
}

double nb_m1_and_1()
{
    return ((double)rand() / (double)((unsigned)RAND_MAX + 1)) * 2 - 1;
}

int index_random(int count)
{
    return (int)((((double)rand() / (double)(unsigned)RAND_MAX + 1))
                 * (double)count)
           - count;
}

double normal(double z)
{
    return exp(-(z * z) / 2) / sqrt(2 * M_PI);
}

double sign(double nb)
{
    return (nb > 0 ? 1 : -1);
}

double *generate_weights(size_t n, size_t prev)
{
    double stddev = 1 / (sqrt(prev));
    return marsaglia(n, 0, stddev);
}

double *marsaglia(size_t count, double mean, double stddev)
{
    double *values = (double *)calloc(count, sizeof(double));
    if (values == NULL)
        return NULL;

    double spare;
    int has_spare = 0;
    for (size_t i = 0; i < count; i++)
    {
        if (has_spare)
        {
            has_spare = 0;
            values[i] = spare * stddev + mean;
        }
        else
        {
            double u, v, s;
            do
            {
                u = (rand() / ((double)RAND_MAX)) * 2.0 - 1.0;
                v = (rand() / ((double)RAND_MAX)) * 2.0 - 1.0;
                s = u * u + v * v;
            } while (s >= 1.0 || s == 0);
            s = sqrt(-2.0 * log(s) / s);
            values[i] = u * s * stddev + mean;
            spare = v * s;
            has_spare = 1;
        }
    }

    return values;
}
