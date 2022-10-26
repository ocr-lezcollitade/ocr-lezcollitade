#include <err.h>
#include <math.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "./cli/parser.h"
#include "./network/network.h"
#include "./solver/solver.h"
#include "./utils/matrices/matrix.h"
#include "./utils/mnist/loader.h"

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
        network_t *net = network_generate(layers, length);
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

static double get_rate(params_t params)
{
    double rate = 0.1;
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
    PRINT_PARAM(params, LENGTH);
}

int main(int argc, char **argv)
{
    UNUSED(print_params);
    char mode;
    params_t params = parse_params(argc, argv, &mode);
    if (params == NULL)
    {
        errx(-1, "missing subcommand");
    }

    signal(SIGINT, int_handler);
    if (mode == TRAIN_MODE)
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

        double rate = get_rate(params);
        int running = keep_running || iterations > 0;
        for (size_t i = 0; running; i++)
        {
            for (size_t j = 0; j < length && (running); j++)
            {
                network_train(&net, inputs[j], outputs[j], rate);
                running = keep_running || i < iterations;
            }
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
        free(params);
    }
    else if (mode == TEST_MODE)
    {

        network_t *net = get_train_network(params);
        matrix_t **inputs;
        matrix_t **outputs;
        size_t length = load_train_data(params, &inputs, &outputs, net->inputs,
            net->layers[net->layer_count - 1]->count);

        printf("starting the test\n");

        size_t succeeded = 0, tests = 0;
        for (size_t j = 0; j < length && keep_running; j++)
        {
            matrix_t *res = compute_results(inputs[j], net);
            int out = network_get_output(res, 0);
            int ex = network_get_output(outputs[j], 0);
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
        free(params);
    }

    return 0;
}
