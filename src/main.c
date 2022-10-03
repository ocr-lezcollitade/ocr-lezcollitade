#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "./network/network.h"
#include "./solver/solver.h"
#include "./utils/matrices/matrix.h"

int main(int argc, char **argv)
{

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
