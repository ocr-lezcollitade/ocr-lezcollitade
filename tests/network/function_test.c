#include <criterion/criterion.h>
#include <criterion/new/assert.h>
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "../../src/network/function.h"
#include "../../src/network/network.h"

TestSuite(network);

Test(network, load_network)
{
    network_t *net = network_load("./tests/samples/test.network");
    double values[] = {1, 1};
    matrix_t *input = mat_create_fill(2, 1, values);
    mat_print(input);

    matrix_t *result = compute_results(input, net);
    printf(
        "test2: {%f, %f} -> ", mat_el_at(input, 0, 0), mat_el_at(input, 1, 0));
    printf("%f\n", mat_el_at(result, 0, 0));
    matrix_free(result);

    mat_set_el(input, 0, 0, 0);
    result = compute_results(input, net);
    printf(
        "test2: {%f, %f} -> ", mat_el_at(input, 0, 0), mat_el_at(input, 1, 0));
    printf("%f\n", mat_el_at(result, 0, 0));

    mat_set_el(input, 1, 0, 0);
    result = compute_results(input, net);
    printf(
        "test2: {%f, %f} -> ", mat_el_at(input, 0, 0), mat_el_at(input, 1, 0));
    printf("%f\n", mat_el_at(result, 0, 0));

    mat_set_el(input, 0, 0, 1);
    result = compute_results(input, net);
    printf(
        "test2: {%f, %f} -> ", mat_el_at(input, 0, 0), mat_el_at(input, 1, 0));
    printf("%f\n", mat_el_at(result, 0, 0));

    network_free(net);
}

TestSuite(random);

#define NMAX 10000000

double test_mean(double *values, int n)
{
    int i;
    double s = 0;

    for (i = 0; i < n; i++)
        s += values[i];
    return s / n;
}

double test_stddev(double *values, int n)
{
    int i;
    double average = test_mean(values, n);
    double s = 0;

    for (i = 0; i < n; i++)
        s += (values[i] - average) * (values[i] - average);
    return sqrt(s / (n - 1));
}

void printHistogram(
    double *values, int n, double low, double high, double delta)
{
    const int width = 50;
    int max = 0;

    // const double low   = -3.05;
    // const double high  =  3.05;
    // const double delta =  0.1;

    int i, j, k;
    int nbins = (int)((high - low) / delta);
    int *bins = (int *)calloc(nbins, sizeof(int));
    if (bins != NULL)
    {
        for (i = 0; i < n; i++)
        {
            int j = (int)((values[i] - low) / delta);
            if (0 <= j && j < nbins)
                bins[j]++;
        }

        for (j = 0; j < nbins; j++)
            if (max < bins[j])
                max = bins[j];

        for (j = 0; j < nbins; j++)
        {
            printf("(%5.2f, %5.2f) |", low + j * delta, low + (j + 1) * delta);
            k = (int)((double)width * (double)bins[j] / (double)max);
            while (k-- > 0)
                putchar('*');
            printf("  %-.1f%%", bins[j] * 100.0 / (double)n);
            putchar('\n');
        }

        free(bins);
    }
}

Test(random, marsaglia)
{
    double *seq;

    double mean = 3, stddev = 0.1;

    srand((unsigned int)time(NULL));

    if ((seq = marsaglia(NMAX, mean, stddev)) != NULL)
    {
        printf("mean = %g, stddev = %g\n\n", test_mean(seq, NMAX),
            test_stddev(seq, NMAX));
        printHistogram(seq, NMAX, 2.7, 3.3, 0.01);
        free(seq);
    }
}
