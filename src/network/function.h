#ifndef UTILS_FUNCTION_H
#define UTILS_FUNCTION_H

#include "../utils/matrices/matrix.h"
#include "network.h"

double cost(matrix_t *target, matrix_t *output);

double cost_derivative(double target, double output);

layer_activation_t get_layer_activation(const char *name);

/**
 *  \brief      Returns a double  random in ]-1; 1[.
 *  \fn double nbm1and1();
 *  \return     The generated random.
 */

double nb_m1_and_1();

/**
 *  \brief      Returns a number in [0; count[;
 *  \fn double int indexRandom(int count);
 *  \param count The maximum bound
 *  \return     The generated random.
 */

int index_random(int count);

/**
 *  \brief      Returns a number following the standard normal distribution;
 *  \fn double normal(double z);
 *  \param z The argument for the standard normal distribution
 *  \return     The number following the standard normal distribution
 */

double normal(double z);

/**
 *  \brief      Returns 1 if the double is positive, -1 otherwise;
 *  \fn double sign(double nb);
 *  \param nb   A double number
 *  \return     Returns 1 if the double is positive, -1 otherwise;
 */

double sign(double nb);

/**
 *  \brief      Returns a double pointer with values following normal
 * distribution;
 *  \fn double * generate_weights(double n);
 *  \param count    Numbers of element for the distribution
 *  \param  previous_layer_count    The number of neurons in the previous
 * layer. \return     a double pointer with values following normal
 * distribution;
 */
// double *generate_weights(double n);
double *generate_weights(size_t count, size_t previous_layer_count);

/**
 *  \brief      Returns an array of normally distributed random numbers using
 *  Marsaglia algorithm.
 *  \fn         double *marsaglia(double n, double mean, double stddev);
 *  \param n    The count of numbers to generate.
 *  \param mean The mean of the generate numbers.
 *  \param stddev   The standard deviation for the generated numbers.
 *  \return     A list of n doubles containing generate numbers.
 */
double *marsaglia(size_t n, double mean, double stddev);

#endif /* !UTILS_FUNCTION_H */
