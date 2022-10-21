#include <err.h>
#include <malloc.h>
#include <string.h>
#include "parser.h"

static params_t create_params(size_t length)
{
    params_t res = (params_t)malloc(length * sizeof(char *));
    for (size_t i = 0; i < length; i++)
    {
        res[i] = NULL;
    }

    return res;
}

static int get_next_key(const char *param)
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
            nextkey = get_next_key(param);
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
    return res;
}
