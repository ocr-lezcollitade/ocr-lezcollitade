#ifndef UTILS_MNIST_LOADER_H
#define UTILS_MNIST_LOADER_H

#include "../matrices/matrix.h"

#define BINARY_MODE "rb"
#define CSV_MODE "r"

/**
 *  \brief      Loads the mnist data set.
 *  \fn         void load_mnist(const char* file
 *  matrix_t ***inputs, matrix_t ***expected,
 *  size_t length, const char *mode)
 *  \param file The file to read
 *  \param inputs   A pointer to the list of matrix_inputs.
 *  \param expected A pointer to the list of expected outputs.
 *  \param start    The line to start at.
 *  \param length   The number of lines to read. If 0, data will be read until
 *  the end.
 *  \param mode     Choose BINARY_MODE for binary files or CSV_MODE for
 *  csv files.
 *  \param inputsize    The number of inputs for 1 dataset.
 *  \param outputsize   The number of outputs for 1 dataset.
 *  \return             The length read.
 */
size_t load_mnist(const char *file, matrix_t ***inputs, matrix_t ***expected,
    size_t start, size_t length, const char *mode, size_t inputsize,
    size_t outputsize);

#endif /* !UTILS_MNIST_LOADER_H */
