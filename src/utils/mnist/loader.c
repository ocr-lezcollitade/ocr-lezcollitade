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
        if (value >= 0)
        {
            // sets at position "value" the value 1
            mat_set_el(expected[readline], value, 0, 1);
        }
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

static size_t load_csv(FILE *file, matrix_t **inputs, matrix_t **expected,
    size_t start, size_t length)
{
    char buffer[READ_LENGTH];
    char number[4] = {};
    int numberi = 0;
    size_t line = 0, readline = 0, readchar = 0;
    while (readline < length)
    {
        if (fgets(buffer, READ_LENGTH, file) == NULL)
            return readline;
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
            else if ((c >= '0' && c <= '9') || (readchar == 0 && c == '-'))
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

    return readline;
}

static void on_number_stop_dynamic(char *number, matrix_t ***expected,
    matrix_t ***inputs, size_t readline, size_t readchar, size_t inputsize,
    size_t outputsize)
{
    int value = atoi(number);
    if (readchar == 0)
    {
        // sets at position "value" the value 1
        *expected = (matrix_t **)realloc(
            *expected, (readline + 1) * sizeof(matrix_t *));
        (*expected)[readline] = matrix_create(outputsize, 1, 0);
        if (value >= 0)
        {
            mat_set_el((*expected)[readline], value, 0, 1);
        }
    }
    else
    {
        if (readchar == 1)
        {
            *inputs = (matrix_t **)realloc(
                *inputs, (readline + 1) * sizeof(matrix_t *));
            (*inputs)[readline] = matrix_create(inputsize, 1, 0);
        }
        mat_set_el((*inputs)[readline], readchar - 1, 0, value / 255.0);
    }

    for (size_t i = 0; i < 4; i++)
    {
        number[i] = 0;
    }
}
static size_t load_csv_dynamic(FILE *file, matrix_t ***inputs,
    matrix_t ***expected, size_t start, size_t inputsize, size_t outputsize)
{
    char buffer[READ_LENGTH];
    char number[4] = {};
    int numberi = 0;
    size_t line = 0, readline = 0, readchar = 0;
    while (1)
    {
        if (fgets(buffer, READ_LENGTH, file) == NULL)
            return readline;
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
                on_number_stop_dynamic(number, expected, inputs, readline,
                    readchar, inputsize, outputsize);
                readchar++;
                numberi = 0;
            }
            else if (c == '\n')
            {
                on_number_stop_dynamic(number, expected, inputs, readline,
                    readchar, inputsize, outputsize);
                readchar = 0;
                readline++;
                numberi = 0;
                break;
            }
            else if ((c >= '0' && c <= '9') || (readchar == 0 && c == '-'))
            {
                number[numberi] = c;
                numberi++;
            }
            else if (c == '\0')
            {
                break;
            }
            else if (c != '\r')
            {
                errx(-1, "Unknown character \'%c\'(%d)", c, c);
            }
        }
    }

    return readline;
}

static void init_vectors(matrix_t **mat, size_t count, size_t length, int fill)
{
    for (size_t i = 0; i < count; i++)
    {
        mat[i] = matrix_create(length, 1, fill);
    }
}

size_t load_mnist(const char *path, matrix_t ***_inputs, matrix_t ***_expected,
    size_t start, size_t length, const char *mode, size_t inputsize,
    size_t outputsize)
{

    if (strcmp(mode, CSV_MODE) && strcmp(mode, BINARY_MODE))
    {
        return 0;
    }

    FILE *file = fopen(path, mode);
    if (file == NULL)
        return 0;

    if (length == 0)
    {
        *_inputs = NULL;
        *_expected = NULL;
        if (strcmp(mode, CSV_MODE) == 0)
            length = load_csv_dynamic(
                file, _inputs, _expected, start, inputsize, outputsize);
        else
            length = 0;
    }
    else
    {
        *_inputs = (matrix_t **)malloc(length * sizeof(matrix_t *));
        if (*_inputs == NULL)
            return 0;
        *_expected = (matrix_t **)malloc(length * sizeof(matrix_t *));
        if (*_expected == NULL)
            return 0;
        init_vectors(*_expected, length, outputsize, 1);
        init_vectors(*_inputs, length, inputsize, 0);
        if (strcmp(mode, CSV_MODE) == 0)
        {
            length = load_csv(file, *_inputs, *_expected, start, length);
        }
        else
        {
            length = 0;
        }
    }

    fclose(file);
    return length;
}
