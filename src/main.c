#include <math.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include "./network/network.h"
#include "./solver/solver.h"
#include "./utils/matrices/matrix.h"
#include "./utils/mnist/loader.h"

static volatile int keep_running = 1;

static void int_handler(int dummy)
{
    keep_running = dummy & 0;
}

int main(int argc, char **argv)
{
    if (argc < 4)
    {
        return -1;
    }
    if (atoi(argv[3]) == 1)
    {
        signal(SIGINT, int_handler);
        matrix_t **inputs;
        matrix_t **expected;

        size_t length = 50000;

        load_mnist(argv[1], &inputs, &expected, 1, length, CSV_MODE, 784, 10);

        /*size_t layers[] = {784, 30, 10};
        network_t *net
            = network_generate(layers, sizeof(layers) / sizeof(size_t));*/
        network_t *net = network_load(argv[2]);

        size_t j = 0;
        while (keep_running)
        {
            for (size_t i = 0; i < length; i++)
            {
                network_train(&net, inputs[i], expected[i], 0.001);
            }
            printf("iteration %zu\n", ++j);
        }

        for (size_t i = 0; i < length; i++)
        {
            matrix_free(expected[i]);
            matrix_free(inputs[i]);
        }
        free(inputs);
        free(expected);

        network_save(argv[2], net);
        printf("saved to %s\n", argv[2]);

        network_free(net);
    }
    else
    {
        network_t *net = network_load(argv[1]);

        matrix_t **inputs;
        matrix_t **expected;
        size_t length = 10000;

        load_mnist(argv[2], &inputs, &expected, 1, length, CSV_MODE, 784, 10);
        size_t succeeded = 0;
        for (size_t i = 0; i < length; i++)
        {
            matrix_t *res = compute_result(inputs[i], net);
            int o1 = network_get_output(res, -1),
                o2 = network_get_output(expected[i], -1);
            if (o1 == o2)
                succeeded++;
            matrix_free(res);
        }

        for (size_t i = 0; i < length; i++)
        {
            matrix_free(inputs[i]);
            matrix_free(expected[i]);
        }
        printf(
            "success rate: %.2f%c\n", 100.0 * (double)succeeded / length, '%');

        free(inputs);
        free(expected);

        network_free(net);
    }

    return 0;

    if (argc == 2)
    {
        printf("%s\n", argv[1]);
        size_t layers[] = {2, 2, 1};
        network_t *net
            = network_generate(layers, sizeof(layers) / sizeof(size_t));

        /*
           network_t *net = network_load(argv[1]);*/
        double values[][2] = {{1, 1}, {1, 0}, {0, 1}, {0, 0}};
        double dexpected[][1] = {{0}, {1}, {1}, {0}};
        matrix_t **inputs = (matrix_t **)malloc(4 * sizeof(matrix_t *));
        matrix_t **expected = (matrix_t **)malloc(4 * sizeof(matrix_t *));
        for (size_t i = 0; i < 4; i++)
        {
            inputs[i] = mat_create_fill(2, 1, values[i]);
            expected[i] = mat_create_fill(1, 1, dexpected[i]);
        }

        for (size_t j = 0; j < 100; j++)
        {
            for (size_t k = 0; k < 100000; k++)
            {
                for (size_t i = 0; i < 4; i++)
                {
                    network_train(&net, inputs[i], expected[i], 0.001);
                }
            }
            printf("advancement: %zu\n", j);
        }

        for (size_t i = 0; i < 4; i++)
        {
            matrix_t *res = compute_result(inputs[i], net);
            printf("%f, %f -> %f\n", mat_el_at(inputs[i], 0, 0),
                mat_el_at(inputs[i], 1, 0), mat_el_at(res, 0, 0));
            matrix_free(res);
        }

        for (size_t i = 0; i < 4; i++)
        {
            matrix_free(inputs[i]);
            matrix_free(expected[i]);
        }
        free(inputs);
        free(expected);

        network_free(net);
    }
    return 0;
}
