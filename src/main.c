#include <stdio.h>
#include <stdlib.h>
#include "./network/network.h"
#include "./solver/solver.h"
#include "./utils/matrices/matrix.h"

int main(int argc, char **argv)
{

    if (argc == 2)
    {
        size_t layers[] = {2, 2, 1};
        network_t *net
            = network_generate(layers, sizeof(layers) / sizeof(size_t));
        double values[] = {1, 1};
        matrix_t *input = mat_create_fill(2, 1, values);
        matrix_t *result = compute_result(input, net);
        printf("result: %f\n", mat_el_at(result, 0, 0));
        matrix_free(input);
        matrix_free(result);
        network_t *copy = network_copy(net);
        network_free(net);
        network_save(argv[1], copy);
        network_free(copy);
    }
    return 0;
}
