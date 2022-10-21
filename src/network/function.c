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

/*static double soft_max(double arg, const network_results_t *res)
{
    double quot = .0;
    matrix_t *outputs = res->outputs[res->network->layer_count];
    for(size_t i = 0; i < outputs->columns; i++) {
        quot += exp(mat_el_at(outputs, i, 0));
    }
    return exp(arg) / quot;
}*/

double output_activation(
    size_t layer, size_t neuron, double arg, const network_results_t *res)
{
    UNUSED(res);
    UNUSED(layer);
    UNUSED(neuron);
    return sigmoid(arg);
}

double output_activation_derivative(
    size_t layer, size_t neuron, double arg, const network_results_t *res)
{
    UNUSED(res);
    UNUSED(layer);
    UNUSED(neuron);
    return sigmoid_derivative(arg);
}

double activation(
    size_t layer, size_t neuron, double arg, const network_results_t *res)
{
    UNUSED(res);
    UNUSED(layer);
    UNUSED(neuron);
    return sigmoid(arg);
}

double activation_derivative(
    size_t layer, size_t neuron, double arg, const network_results_t *res)
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
