#ifndef CLI_PARSER_H
#define CLI_PARSER_H

#define INPUT_NETWORK 0
#define DATA INPUT_NETWORK + 1
#define FORMAT DATA + 1
#define GENERATE FORMAT + 1
#define OUTPUT_NETWORK GENERATE + 1
#define RATE OUTPUT_NETWORK + 1
#define ITERATIONS RATE + 1
#define START ITERATIONS + 1
#define LENGTH START + 1
#define OUTPUT_ACTIVATION LENGTH + 1
#define ACTIVATION OUTPUT_ACTIVATION + 1
#define VERBOSE ACTIVATION + 1

#define TRAIN_PARAMETER_SIZE VERBOSE + 1

/**
 *  \brief      A hash table of params.
 */
typedef char **params_t;

/**
 *  \brief      Parses the train parameters.
 *  \fn         params_t parse_train_params(int argc, char **argv);
 *  \param argc The number of strings in the argv array.
 *  \param argv The list of strings.
 *  \return     The hash table of params.
 */
params_t parse_train_params(int argc, char **argv);

/**
 *  \brief      Parses the test parameters.
 *  \fn         params_t parse_test_params(int argc, char **argv)
 *  \param argc The number of arguments in the array.
 *  \param argv The array of parameters.
 *  \return     The parameters hash table.
 */
params_t parse_test_params(int argc, char **argv);

#define TRAIN_MODE 0
#define TEST_MODE 1

/**
 *  \brief      Parses all the params.
 *  \fn         params_t parse_params(int argc, char **argv, char *mode)
 *  \param argc The number of strings in the argv array.
 *  \param argv The array of param strings.
 *  \param[out] mode    The mode: either TRAIN_MODE or RUN_MODE.
 *  \return     The hash table of params.
 */
params_t parse_params(int argc, char **argv, char *mode);

#endif /* !CLI_PARSER_H */
