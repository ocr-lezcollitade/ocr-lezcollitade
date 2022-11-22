#include <err.h>
#include <malloc.h>
#include <string.h>
#include "parser.h"

#define UNUSED(x) (void)(x)

static params_t create_params(size_t length)
{
    params_t res = (params_t)malloc(length * sizeof(char *));
    for (size_t i = 0; i < length; i++)
    {
        res[i] = NULL;
    }

    return res;
}

static int get_next_key_convert(const char *param)
{
    int nextkey = -1;
    if (strcmp("-i", param) == 0 || strcmp("--input-network", param) == 0)
    {
        nextkey = INPUT_NETWORK;
    }
    else if (strcmp("--grid", param) == 0)
    {
        nextkey = GRID_PATH;
    }
    else if (strcmp("-d", param) == 0 || strcmp("--data", param) == 0)
    {
        nextkey = IMG_PATH;
    }
    else if (strcmp("--mode", param) == 0)
    {
        nextkey = CONVERT_MODE;
    }
    else
    {
        errx(-1, "Unknown flag %s", param);
    }
    return nextkey;
}

static params_t _parse_convert_params(int argc, char **argv)
{

    params_t res = create_params(TRAIN_PARAMETER_SIZE);
    int nextkey = -1;
    for (int i = 0; i < argc; i++)
    {
        char *param = argv[i];
        if (nextkey == -1)
        {
            nextkey = get_next_key_convert(param);
        }
        else
        {
            res[nextkey] = param;
            nextkey = -1;
        }
    }

    return res;
}

static int get_next_key_network(const char *param, params_t params)
{
    int nextkey = -1;
    if (strcmp("-i", param) == 0 || strcmp("--input-network", param) == 0)
    {
        nextkey = INPUT_NETWORK;
    }
    else if (strcmp("-o", param) == 0 || strcmp("--output", param) == 0)
    {
        nextkey = OUTPUT_NETWORK;
    }
    else if (strcmp("-d", param) == 0 || strcmp("--data", param) == 0)
    {
        nextkey = DATA;
    }
    else if (strcmp("--generate", param) == 0)
    {
        nextkey = GENERATE;
    }
    else if (strcmp("-F", param) == 0 || strcmp("--format", param) == 0)
    {
        nextkey = FORMAT;
    }
    else if (strcmp("--rate", param) == 0)
    {
        nextkey = RATE;
    }
    else if (strcmp("--iterations", param) == 0 || strcmp("-c", param) == 0)
    {
        nextkey = ITERATIONS;
    }
    else if (strcmp("--start", param) == 0)
    {
        nextkey = START;
    }
    else if (strcmp("--length", param) == 0)
    {
        nextkey = LENGTH;
    }
    else if (strcmp("--activation", param) == 0)
    {
        nextkey = ACTIVATION;
    }
    else if (strcmp("--output-activation", param) == 0)
    {
        nextkey = OUTPUT_ACTIVATION;
    }
    else if (strcmp("--verbose", param) == 0 || strcmp("-v", param) == 0)
    {
        params[VERBOSE] = (char *)0x1;
        nextkey = -1;
    }
    else
    {
        errx(-1, "Unknown flag %s", param);
    }
    return nextkey;
}

static params_t _parse_params(int argc, char **argv)
{

    params_t res = create_params(TRAIN_PARAMETER_SIZE);
    int nextkey = -1;
    for (int i = 0; i < argc; i++)
    {
        char *param = argv[i];
        if (nextkey == -1)
        {
            nextkey = get_next_key_network(param, res);
        }
        else
        {
            res[nextkey] = param;
            nextkey = -1;
        }
    }

    return res;
}

params_t parse_train_params(int argc, char **argv)
{
    params_t params = _parse_params(argc, argv);
    // TODO : check parameters validity
    return params;
}

params_t parse_test_params(int argc, char **argv)
{

    params_t params = _parse_params(argc, argv);
    // TODO : check parameters validity
    return params;
}

params_t parse_convert_params(int argc, char **argv)
{
    params_t params = _parse_convert_params(argc, argv);
    // TODO : check parameters validity
    return params;
}

params_t parse_params(int argc, char **argv, char *mode)
{
    if (argc < 2)
    {
        return NULL;
    }

    params_t res = NULL;
    char *subcommand = argv[1];
    if (strcmp(subcommand, "train") == 0)
    {
        *mode = TRAIN_MODE;
        res = parse_train_params(argc - 2, argv + 2);
    }
    else if (strcmp(subcommand, "test") == 0)
    {
        *mode = TEST_MODE;
        res = parse_test_params(argc - 2, argv + 2);
    }
    else if (strcmp(subcommand, "convert") == 0)
    {
        *mode = CONVERT_MODE;
        res = parse_convert_params(argc - 2, argv + 2);
    }
    return res;
}
