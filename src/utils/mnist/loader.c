#include <err.h>
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "loader.h"

#define OUTPUT_SIZE 10
#define READ_LENGTH 1024

static void on_number_stop(char *number, matrix_t **expected,
    matrix_t **inputs, size_t readline, size_t readchar)
{
    int value = atoi(number);
    if (readchar == 0)
    {
        mat_set_el(expected[readline], value, 0, 1);
    }
    else
    {
        mat_set_el(inputs[readline], readchar - 1, 0, value / 255.0);
    }

    for (size_t i = 0; i < 4; i++)
    {
        number[i] = 0;
    }
}

static int load_csv(FILE *file, matrix_t **inputs, matrix_t **expected,
    size_t start, size_t length)
{
    char buffer[READ_LENGTH];
    char number[4] = {};
    int numberi = 0;
    size_t line = 0, readline = 0, readchar = 0;
    while (readline < length)
    {
        if (fgets(buffer, READ_LENGTH, file) == NULL)
            return 0;
        for (size_t i = 0; i < READ_LENGTH; i++)
        {
            char c = buffer[i];
            if (line < start)
            {
                if (c == '\n')
                {
                    line++;
                    break;
                }
                continue;
            }
            if (c == ',')
            {
                on_number_stop(number, expected, inputs, readline, readchar);
                readchar++;
                numberi = 0;
            }
            else if (c == '\n' || c == '\r')
            {
                on_number_stop(number, expected, inputs, readline, readchar);
                readchar = 0;
                readline++;
                numberi = 0;
                break;
            }
            else if (c >= '0' && c <= '9')
            {
                number[numberi] = c;
                numberi++;
            }
            else if (c == '\0')
            {
                break;
            }
            else
            {
                errx(-1, "Unknown character \'%c\'(%d)", c, c);
            }
        }
    }

    return 0;
}

static void init_vectors(matrix_t **mat, size_t count, size_t length, int fill)
{
    for (size_t i = 0; i < count; i++)
    {
        mat[i] = matrix_create(length, 1, fill);
    }
}

int load_mnist(const char *path, matrix_t ***_inputs, matrix_t ***_expected,
    size_t start, size_t length, const char *mode, size_t inputsize,
    size_t outputsize)
{

    if (strcmp(mode, CSV_MODE) && strcmp(mode, BINARY_MODE))
    {
        return -1;
    }

    FILE *file = fopen(path, mode);
    if (file == NULL)
        return -1;
    *_inputs = (matrix_t **)malloc(length * sizeof(matrix_t *));
    if (*_inputs == NULL)
        return -1;
    init_vectors(*_inputs, length, inputsize, 0);
    *_expected = (matrix_t **)malloc(length * sizeof(matrix_t *));
    if (*_expected == NULL)
        return -1;
    init_vectors(*_expected, length, outputsize, 1);

    int result;
    if (strcmp(mode, CSV_MODE) == 0)
    {
        result = load_csv(file, *_inputs, *_expected, start, length);
    }
    else
    {
        result = -1;
    }
    fclose(file);
    return result;
}
