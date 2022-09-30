#ifndef UTILS_NETWORK_H
#define UTILS_NETWORK_H
#include "../utils/matrices/matrix.h"

/**
 *  \brief          A struct representing a neuron in the network.
 *  \struct         neuron_t
 */
typedef struct
{
    matrix_t *weights; /*! The row vector of the weights of each synapse before
                          the neuron */
    double bias;       /*! the bias of the neuron */
} neuron_t;

/**
 *  \brief          Creates a neuron.
 *  \fn             neuron_t *neuron_create(matrix_t *weights, double bias)
 *  \param weights  The weights of the neuron.
 *  \param bias     The bias of the neuron.
 *  \return         The instantiated neuron.
 */
neuron_t *neuron_create(matrix_t *weights, double bias);

/**
 *  \brief          Frees the allocated neuron.
 *  \fn             void neuron_free(neuron_t *neuron)
 *  \param neuron   The neuron to free.
 */
void neuron_free(neuron_t *neuron);

/**
 *  \brief          A struct representing a layer in the network.
 *  \struct layer_t
 */
typedef struct
{
    neuron_t **neurons; /*! The allocated neurons in the layer. */
    size_t count;       /*! The number of neurons in the layer. */
} layer_t;

/**
 *  \brief          Creates filled layer from an array of weights,
 *  and an array of biases.
 *  \fn             layer_t *layer_create_from_mat(
 *                                                  matrix_t **weights,
 *                                                  double *biases,
 *                                                  size_t count
 *                                                )
 *  \param weights  The array of weights for each neuron in the layer.
 *  \param biases   The array of biases for each neuron in the layer.
 *  \param count    The number of elements to be created, hence the size of
 *  weights and biases.
 *  \return         The created layer.
 */
layer_t *layer_create_from_mat(
    matrix_t **weights, double *biases, size_t count);

/**
 *  \brief          Creates a layer from an array of neurons.
 *  \fn             layer_t *layer_create(neuron_t **neurons, size_t count)
 *  \param neurons  The array of neurons in the layer.
 *  \param count    The count of neurons in the layer.
 *  \return         The created layer.
 */
layer_t *layer_create(neuron_t **neurons, size_t count);

/**
 *  \brief          Frees an allocated layer.
 *  \fn             void layer_free(layer_t *layer)
 *  \param layer    The layer to be freed.
 */
void layer_free(layer_t *layer);

/**
 *  \brief          A struct representing a neural network.
 *  \struct network_t
 */
typedef struct
{
    layer_t **layers;   /*! The array of layers in the network. */
    size_t layer_count; /*! The count of layers in the network. */
    size_t inputs;      /*! The number of neurons in the input layer */
} network_t;

/**
 *  \brief          Creates a network from an array of layers.
 *  \fn             network_t *network_create(
 *                                      layer_t **layers,
 *                                      size_t size,
 *                                      size_t inputs
 *  )
 *  \param layers   The array of layers in the network.
 *  \param size     The number of layers in the array.
 *  \param inputs   The number of neurons in the input layer.
 *  \return         The created network.
 */
network_t *network_create(layer_t **layers, size_t layer_count, size_t inputs);

/**
 *  \brief          Generates a network based on given neuron count.
 *  \fn             network_t *network_generate(size_t *neurons, size_t size)
 *  \param neurons  An array containing the number of neurons at each layer
 * including input layer. \param size     The length of the array, hence the
 * number of layers plus the input layer. \return         The generated
 * network.
 */
network_t *network_generate(size_t *neurons, size_t size);

/**
 *  \brief          Loads the network from a file.
 *  \param file     The path of the file to load.
 *  \return         The loaded network.
 *  The file format:
 *  The file will start with two comma-separated integer indicating
 *  the number of layers and the number of input neurons.
 *  Each layer will start by the number of neurons in the layer.
 *  Then each neuron information will be seperated by a newline char.
 *  Neuron format: <weight1>, ... , <weightn>, <bias>
 *  Example for xor network with random biases and weights:
 *  2,2 # The number of layers and the number of input neurons.
 *  2 # The number of neurons in layer 1
 *  1,1,-0.5 # neuron 1,1 weights are [1, 1] and the bias is -0.5
 *  -1,-1,1.5
 *  1 # The number of neurons in layer 2
 *  1, 1, 0.5
 */
network_t *network_load(const char *file);

/**
 *  \brief          Saves a network to a file.
 *  \fn             void network_save(const char *file, network_t *network)
 *  \param file     The file to save the network to.
 *  \param network  The network to be saved.
 */
void network_save(const char *file, network_t *network);

/**
 *  \brief          Frees the allocated network.
 *  \fn             void network_free(network_t *network)
 *  \param network  The network to be freed.
 */
void network_free(network_t *network);

/**
 *  \brief          Computes all the weights of one layer from the input.
 *  \fn             matrix_t *compute_layer(matrix_t *inputs, layer_t *layer)
 *  \param inputs   The column vector of inputs from the previous layer.
 *  \param layer    The layer to compute.
 *  \return         The column vector of outputs.
 */
matrix_t *compute_layer(matrix_t *values, layer_t *layer);

/**
 *  \brief          Computes the result of the input by the network.
 *  \fn             matrix_t *compute_result(matrix_t *inputs,
 *                                              network_t *network)
 *  \param inputs   The column vector of the values.
 *  \param network  The network to apply the values to.
 *  \return         The column vector of the computed results.
 */
matrix_t *compute_result(matrix_t *values, network_t *network);

/**
 *  \brief          Computes the result while saving the result of each layers.
 *  \fn             matrix_t *compute_result_save(
 *      matrix_t **partial_results,
 *      network_t *network
 *  )
 *  \param partial_results  The array of all the results of each layer and the
 * inputs. It is expected to be of the size of the layer_count + 1, and the
 * first matrix_t is expected to contain the inputs already. \param network The
 * network to compute the results on.
 */
void compute_result_save(matrix_t **partial_results, network_t *network);

/**
 *  \brief          Create a deep copied network.
 *  \fn             network_t *network_copy(network_t *old);
 *  \param old      The network to copy.
 *  \return         The copied network.
 */
network_t *network_copy(network_t *old);

#endif /* !UTILS_NETWORK_H */
