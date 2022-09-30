#include <math.h>
#include <stdlib.h>
#include "function.h"

double sigmoid(double x)
{
    return 1 / (1 + exp(-x));
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
