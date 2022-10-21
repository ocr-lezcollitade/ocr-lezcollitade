#ifndef UTILS_NETWORK_H
#define UTILS_NETWORK_H
#include "../utils/matrices/matrix.h"

struct network_results_t;
typedef struct network_results_t network_results_t;

/**
 *  \brief          The activation function type;
 *  \param layer    The index of the layer the
 *  activation function is called in.
 *  \param neuron   The index of the neuron the
 *  activation function is called in.
 *  \param input    The value of the neuron before the activation function.
 *  \param results  The network_results_structures.
 */
typedef double (*activation_t)(size_t layer, size_t neuron, double input,
    const network_results_t *results);
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
 *  \brief          A struct representing a layer in the network.
 *  \struct layer_t
 */
typedef struct
{
    neuron_t **neurons;       /*! The allocated neurons in the layer. */
    size_t count;             /*! The number of neurons in the layer. */
    activation_t activation;  /*! The activation function for the layer */
    activation_t dactivation; /*! The derivative of the activation function */

} layer_t;

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
 *  \brief          The results of the network.
 *  \struct         network_results_t
 */
struct network_results_t
{
    matrix_t **outputs;
    /*! The outputs of all layers. The size of the list is
     * layer_count + 1 and the first matrix is the inputs.*/
    matrix_t **preactivation;
    /*! The values of all layers before the actvation function. */
    network_t *network; /*! The linked network */
};

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
 *  \brief          Creates filled layer from an array of weights,
 *  and an array of biases.
 *  \fn             layer_t *layer_create_from_mat(
 *                                                  matrix_t **weights,
 *                                                  double *biases,
 *                                                  size_t count,
 *                                                  activation_t f,
 *                                                  activation_t d
 *                                                )
 *  \param weights  The array of weights for each neuron in the layer.
 *  \param biases   The array of biases for each neuron in the layer.
 *  \param count    The number of elements to be created, hence the size of
 *  weights and biases.
 *  \param f        The activation function.
 *  \param d        The derivative of the activation function.
 *  \return         The created layer.
 */
layer_t *layer_create_from_mat(matrix_t **weights, double *biases,
    size_t count, activation_t f, activation_t d);

/**
 *  \brief          Creates a layer from an array of neurons.
 *  \fn             layer_t *layer_create(neuron_t **neurons, size_t count)
 *  \param neurons  The array of neurons in the layer.
 *  \param count    The count of neurons in the layer.
 *  \param f        The activation function of the layer.
 *  \param d        The derivative of the activation function.
 *  \return         The created layer.
 */
layer_t *layer_create(
    neuron_t **neurons, size_t count, activation_t f, activation_t d);

/**
 *  \brief          Frees an allocated layer.
 *  \fn             void layer_free(layer_t *layer)
 *  \param layer    The layer to be freed.
 */
void layer_free(layer_t *layer);

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
 *  \fn             void compute_layer(matrix_t *results, layer_t *layer
 *  mat_transform_t activation)
 *  \param inputs   The column vector of inputs from the previous layer.
 *  \param layer    The layer to compute.
 *  \param layeri   The index of the layer.
 *  \return         The column vector of outputs.
 */
void compute_layer(network_results_t *results, layer_t *layer, size_t layeri);

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
 *  \brief          Create a deep copied network.
 *  \fn             network_t *network_copy(network_t *old);
 *  \param old      The network to copy.
 *  \return         The copied network.
 */
network_t *network_copy(network_t *old);

void network_train(
    network_t **pnet, matrix_t *inputs, matrix_t *target, double rate);

/**
 *  \brief          Return the calculated output.
 *  \fn             int network_get_output(matrix_t *outputs, double threshold)
 *  \param outputs  The matrix of outputs given by the network.
 *  \param threshold    The threshold starting from which the result will be
 *  considered as valid.
 *  \return         The calculated output or -1 if none above the threshold.
 */
int network_get_output(matrix_t *outputs, double threshold);

/**
 *  \brief          Computes the result while saving the result of each layers.
 *  \fn             matrix_t *compute_result_with_save(
 *      network_results_t *results,
 *      network_t *network
 *  )
 *  \param results  The results of the network.
 */
network_results_t *compute_result_with_save(
    matrix_t *inputs, network_t *network);

/**
 *  \brief          Creates a network_results struct.
 *  \fn             network_results_t *results_create(size_t layer_count)
 *  \param layer_count  The number of layers in the network.
 *  \return         The created structure.
 */
network_results_t *results_create(size_t layer_count);

/**
 *  \brief          Frees the network results struct.
 *  \fn             void results_free(network_results_t *results)
 *  \param results  The results struct to free.
 */
void results_free(network_results_t *results);
#endif /* !UTILS_NETWORK_H */
