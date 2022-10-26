#ifndef UTILS_FUNCTION_H
#define UTILS_FUNCTION_H

#include "../utils/matrices/matrix.h"
#include "network.h"

double activation(
    size_t layer, size_t neuron, double arg, network_results_t *results);

double activation_derivative(
    size_t layer, size_t neuron, double arg, network_results_t *results);

double output_activation(
    size_t layer, size_t neuron, double arg, network_results_t *results);

double output_activation_derivative(
    size_t layer, size_t neuron, double arg, network_results_t *results);

double cost(matrix_t *target, matrix_t *output);

double cost_derivative(double target, double output);

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
 * distribution; \fn double * generate_weights(double n); \param n    Numbers
 * of element for the distribution \return     a double pointer with values
 * following normal distribution;
 */

double *generate_weights(double n);
#endif /* !UTILS_FUNCTION_H */
