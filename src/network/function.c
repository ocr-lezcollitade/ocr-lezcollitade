#include <math.h>
#include <stdlib.h>
#include "function.h"

#define UNUSED(x) (void)(x)

static double sigmoid(double x)
{
    return 1 / (1 + exp(-x));
}

static double sigmoid_derivative(double x)
{
    return x * (1 - x);
}

static double soft_max(size_t layeri, size_t neuron, network_results_t *res)
{
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
        if (neuron == layer->count - 1)
        {
            free(cache);
        }
    }
    return value / sum;
}

static double soft_max_derivative(double arg)
{
    return arg * (1 - arg);
}

double output_activation(
    size_t layer, size_t neuron, double arg, network_results_t *res)
{
    UNUSED(arg);
    return soft_max(layer, neuron, res);
}

double output_activation_derivative(
    size_t layer, size_t neuron, double arg, network_results_t *res)
{
    UNUSED(res);
    UNUSED(layer);
    UNUSED(neuron);
    return soft_max_derivative(arg);
}

double activation(
    size_t layer, size_t neuron, double arg, network_results_t *res)
{
    UNUSED(res);
    UNUSED(layer);
    UNUSED(neuron);
    return sigmoid(arg);
}

double activation_derivative(
    size_t layer, size_t neuron, double arg, network_results_t *res)
{
    UNUSED(res);
    UNUSED(layer);
    UNUSED(neuron);
    return sigmoid_derivative(arg);
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

double *generate_weights(double n)
{
    size_t index = 0;
    double *values = (double *)malloc(sizeof(double) * n);
    for (int i = -n / 2; i < n / 2; i++)
    {
        double nb = normal(nb_m1_and_1() * 2) * sign(i);
        values[index] = nb;
        index++;
    }

    // SHUFFLE

    for (int i = 0; i < 100; i++)
    {
        int i1 = index_random(n);
        int i2 = index_random(n);
        double temp = values[i1];
        values[i1] = values[i2];
        values[i2] = temp;
    }
    return values;
}
