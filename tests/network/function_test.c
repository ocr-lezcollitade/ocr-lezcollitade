#include <criterion/criterion.h>
#include <criterion/new/assert.h>
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include "../../src/network/function.h"
#include "../../src/network/network.h"

TestSuite(network);

Test(network, load_network)
{
    network_t *net = network_load("./tests/samples/test.network");
    double values[] = {1, 1};
    matrix_t *input = mat_create_fill(2, 1, values);
    mat_print(input);

    matrix_t *result = compute_result(input, net);
    printf(
        "test2: {%f, %f} -> ", mat_el_at(input, 0, 0), mat_el_at(input, 1, 0));
    printf("%f\n", mat_el_at(result, 0, 0));
    matrix_free(result);

    mat_set_el(input, 0, 0, 0);
    result = compute_result(input, net);
    printf(
        "test2: {%f, %f} -> ", mat_el_at(input, 0, 0), mat_el_at(input, 1, 0));
    printf("%f\n", mat_el_at(result, 0, 0));

    mat_set_el(input, 1, 0, 0);
    result = compute_result(input, net);
    printf(
        "test2: {%f, %f} -> ", mat_el_at(input, 0, 0), mat_el_at(input, 1, 0));
    printf("%f\n", mat_el_at(result, 0, 0));

    mat_set_el(input, 0, 0, 1);
    result = compute_result(input, net);
    printf(
        "test2: {%f, %f} -> ", mat_el_at(input, 0, 0), mat_el_at(input, 1, 0));
    printf("%f\n", mat_el_at(result, 0, 0));

    network_free(net);
}
