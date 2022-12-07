#include <err.h>
#include <gtk/gtk.h>
#include <math.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "./cli/parser.h"
#include "./network/function.h"
#include "./network/network.h"
#include "./solver/solver.h"
#include "./ui/ui.h"
#include "./utils/img_loader/loader.h"
#include "./utils/matrices/matrix.h"
#include "./utils/mnist/loader.h"

#define DEFAULT_RATE 0.1

#define UNUSED(x) (void)(x)

static volatile int keep_running = 1;

static void int_handler(int dummy)
{
    keep_running = dummy & 0;
}

static size_t *get_layers(const char *layers, size_t *length)
{
    size_t *res = NULL;
    size_t buffer = 0;
    *length = 0;
    for (size_t i = 0; 1; i++)
    {
        char c = layers[i];
        if (c >= '0' && c <= '9')
        {
            buffer = buffer * 10 + (c - '0');
        }
        else if (c == ',')
        {
            *length = (*length) + 1;
            res = (size_t *)realloc(res, (*length) * sizeof(size_t));
            res[(*length) - 1] = buffer;
            buffer = 0;
        }
        else if (c == '\0')
        {
            *length = (*length) + 1;
            res = (size_t *)realloc(res, (*length) * sizeof(size_t));
            res[(*length) - 1] = buffer;
            break;
        }
    }

    return res;
}

static void get_param_activations(
    params_t params, layer_activation_t *hidden, layer_activation_t *output)
{
    *hidden = get_layer_activation(params[ACTIVATION]);
    *output = get_layer_activation(params[OUTPUT_ACTIVATION]);
}

static network_t *get_train_network(params_t params)
{
    if (params[INPUT_NETWORK] != NULL)
    {
        return network_load(params[INPUT_NETWORK]);
    }
    else if (params[GENERATE] != NULL)
    {
        size_t length;
        size_t *layers = get_layers(params[GENERATE], &length);
        layer_activation_t hidden, output;
        get_param_activations(params, &hidden, &output);
        network_t *net = network_generate(layers, length, hidden, output);
        free(layers);
        return net;
    }
    return NULL;
}

static size_t load_train_data(params_t params, matrix_t ***inputs,
    matrix_t ***expected, size_t inputsize, size_t outputsize)
{
    size_t length = 0;
    size_t start = 0;
    if (params[LENGTH] == NULL || sscanf(params[LENGTH], "%zu", &length) == 0)
    {
        length = 0;
    }

    if (params[START] == NULL || sscanf(params[START], "%zu", &start) == 0)
    {
        start = 0;
    }

    if (params[DATA] == NULL)
    {
        errx(-1, "missing parameter data");
    }

    char *format;
    if (params[FORMAT] == NULL)
    {
        errx(-1, "missing parameter format");
    }
    else if (strcmp(params[FORMAT], "csv") == 0)
    {
        format = CSV_MODE;
    }
    else if (strcmp(params[FORMAT], "bin") == 0)
    {
        format = BINARY_MODE;
    }
    else
    {
        errx(-1, "wrong format provided, should be of <csv|bin>");
    }
    size_t l = load_mnist(params[DATA], inputs, expected, start, length,
        format, inputsize, outputsize);
    if (l < length)
        return length;
    else
        return l;
}

static int get_verbose(params_t params)
{
    return params[VERBOSE] == NULL ? 0 : 1;
}

static double get_rate(params_t params, double val)
{
    double rate = val;
    if (params[RATE] != NULL)
    {
        rate = atof(params[RATE]);
    }
    return rate;
}

#define PRINT_PARAM(params, key) \
    printf("%s: \'%d\' -> %s\n", #key, key, params[key])

static void print_params(params_t params)
{
    printf("params: \n");
    PRINT_PARAM(params, INPUT_NETWORK);
    PRINT_PARAM(params, DATA);
    PRINT_PARAM(params, FORMAT);
    PRINT_PARAM(params, GENERATE);
    PRINT_PARAM(params, OUTPUT_NETWORK);
    PRINT_PARAM(params, RATE);
    PRINT_PARAM(params, ITERATIONS);
    PRINT_PARAM(params, START);
    PRINT_PARAM(params, VERBOSE);
    PRINT_PARAM(params, LENGTH);
    PRINT_PARAM(params, GRID_PATH);
    PRINT_PARAM(params, IMG_PATH);
    PRINT_PARAM(params, CONVERT_MODE);
}

static void show_results(
    matrix_t *inputs, matrix_t *expected, int ex, matrix_t *outputs, int out)
{

    printf("===TEST===\n");
    printf("inputs:\n");
    mat_print(inputs);
    printf("outputs: %d\n", out);
    mat_print(outputs);
    printf("expected: %d\n", ex);
    mat_print(expected);
    printf("\n");
}

static void train_xor(params_t params)
{
    network_t *net = get_train_network(params);
    double invalues[][2] = {{1, 0}, {0, 1}, {1, 1}, {0, 0}};
    double outvalues[][1] = {{1}, {1}, {0}, {0}};
    matrix_t **inputs
        = (matrix_t **)calloc(sizeof(invalues), sizeof(matrix_t *));
    size_t length = sizeof(invalues) / sizeof(double[2]);
    for (size_t i = 0; i < length; i++)
    {
        inputs[i] = mat_create_fill(2, 1, invalues[i]);
    }

    matrix_t **outputs
        = (matrix_t **)calloc(sizeof(outvalues), sizeof(matrix_t *));
    for (size_t i = 0; i < length; i++)
    {
        outputs[i] = mat_create_fill(1, 1, outvalues[i]);
    }
    for (size_t i = 0; i < 100000; i++)
    {
        for (size_t j = 0; j < length; j++)
        {
            network_train(&net, inputs[j], outputs[j], 0.1);
        }
    }

    for (size_t j = 0; j < length; j++)
    {
        matrix_t *res = compute_results(inputs[j], net);
        int out = network_get_output(res, 0.8);
        int ex = network_get_output(outputs[j], 0.8);
        show_results(inputs[j], outputs[j], ex, res, out);
        matrix_free(res);
    }
    network_save(params[OUTPUT_NETWORK], net);
    network_free(net);

    for (size_t i = 0; i < length; i++)
    {
        matrix_free(inputs[i]);
        matrix_free(outputs[i]);
    }

    free(inputs);
    free(outputs);
}

static void train(params_t params)
{
    network_t *net = get_train_network(params);
    matrix_t **inputs;
    matrix_t **outputs;
    size_t length = load_train_data(params, &inputs, &outputs, net->inputs,
        net->layers[net->layer_count - 1]->count);

    size_t iterations = 0;
    if (params[ITERATIONS] == NULL
        || sscanf(params[ITERATIONS], "%zu", &iterations) == 0)
    {
        iterations = 0;
    }
    else
    {
        keep_running = 0;
    }
    printf("starting the training\n");

    double rate = get_rate(params, DEFAULT_RATE);
    int running = keep_running || iterations > 0;
    for (size_t i = 0; running; i++)
    {
        size_t j;
        for (j = 0; j < length && (running); j++)
        {
            network_train(&net, inputs[j], outputs[j], rate);
            running = keep_running || i < iterations;
        }
        if (j == length)
            printf("iteration %zu\n", i + 1);
    }

    printf("saving to %s\n", params[OUTPUT_NETWORK]);
    network_save(params[OUTPUT_NETWORK], net);
    network_free(net);
    for (size_t i = 0; i < length; i++)
    {
        matrix_free(inputs[i]);
        matrix_free(outputs[i]);
    }

    free(inputs);
    free(outputs);
}

static void test(params_t params)
{
    network_t *net = get_train_network(params);
    matrix_t **inputs;
    matrix_t **outputs;
    size_t length = load_train_data(params, &inputs, &outputs, net->inputs,
        net->layers[net->layer_count - 1]->count);
    int verbose = get_verbose(params);

    printf("starting the test\n");

    size_t succeeded = 0, tests = 0;
    for (size_t j = 0; j < length && keep_running; j++)
    {
        matrix_t *res = compute_results(inputs[j], net);
        int out = network_get_output(res, 0.8);
        int ex = network_get_output(outputs[j], 0.8);
        if (verbose)
            show_results(inputs[j], outputs[j], ex, res, out);
        matrix_free(res);
        if (out == ex)
            succeeded++;
        tests++;
    }
    printf("rate: %.2f%c\n", 100.0 * succeeded / (double)tests, '%');

    network_free(net);
    for (size_t i = 0; i < length; i++)
    {
        matrix_free(inputs[i]);
        matrix_free(outputs[i]);
    }

    free(inputs);
    free(outputs);
}

static int get_convert_mode(params_t params)
{

    int res = -1;
    char *mode = params[CONVERT_MODE];
    if (mode != NULL)
    {
        if (strcmp(mode, "single") == 0)
        {
            res = 1;
        }
        else if (strcmp(mode, "multi") == 0)
        {
            res = 2;
        }
    }

    if (res < 0)
        errx(-1, "Invalid value for parameter --mode");

    return res;
}

static void cli_convert(params_t params)
{
    int convert_mode = get_convert_mode(params);
    double default_rate = 0.4;
    double rate = get_rate(params, default_rate);
    network_t *net = network_load(params[INPUT_NETWORK]);
    if (convert_mode == 1)
        test_single(net, params[IMG_PATH], rate);
    else
    {
        // 9 a change qd on aura le boutton de l'ui
        int *grid = convert_multi(params[IMG_PATH], net, rate);
        int *original = copy_grid(grid, 9);
        solve(grid, 9);
        writeGrid(params[GRID_PATH], grid, 9);
        free(grid);
        free(original);
    }
    network_free(net);
}

int main(int argc, char **argv)
{

    // Initializes the random generator.
    srand((unsigned int)time(NULL));

    UNUSED(print_params);
    UNUSED(show_results);
    UNUSED(train_xor);
    char mode;
    params_t params = parse_params(argc, argv, &mode);
    if (argc == 2)
    {
        int *grid = readGrid(argv[1], 16);
        solve(grid, 16);
        writeGrid(argv[1], grid, 16);
        free(grid);
        return 0;
    }
    if (params == NULL)
    {
        open_ui();
        return 0;
    }
    signal(SIGINT, int_handler);
    if (mode == TRAIN_MODE)
    {
        train(params);
    }
    else if (mode == TEST_MODE)
    {
        test(params);
    }
    else if (mode == CONVERT_MODE)
    {
        cli_convert(params);
    }
    free(params);

    return 0;
}
