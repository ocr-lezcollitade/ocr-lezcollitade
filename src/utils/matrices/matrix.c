#include <err.h>
#include <malloc.h>
#include <stdio.h>
#include "matrix.h"

double mat_el_at(matrix_t *mat, size_t i, size_t j)
{
    size_t index = i * mat->columns + j;
    return mat->_elements[index];
}

void mat_set_el(matrix_t *mat, size_t i, size_t j, double value)
{
    size_t index = i * mat->columns + j;
    mat->_elements[index] = value;
}

matrix_t *matrix_create(size_t rows, size_t columns, int fill)
{
    matrix_t *res = (matrix_t *)malloc(sizeof(matrix_t));
    if (res == NULL)
        return NULL;
    size_t count = rows * columns;
    res->_elements = (double *)malloc(count * sizeof(double));
    if (res->_elements == NULL)
        return NULL;
    if (fill)
    {
        for (size_t i = 0; i < count; i++)
            res->_elements[i] = 0;
    }

    res->rows = rows;
    res->columns = columns;
    return res;
}

matrix_t *mat_create_fill(size_t rows, size_t columns, double *elements)
{
    matrix_t *res = matrix_create(rows, columns, 0);
    mat_fill(res, elements);
    return res;
}

void matrix_free(matrix_t *mat)
{
    free(mat->_elements);
    free(mat);
}

matrix_t *mat_product(matrix_t *A, matrix_t *B)
{
    if (A->columns != B->rows)
        return NULL;

    matrix_t *res = matrix_create(A->rows, B->columns, 0);
    if (res == NULL)
        return NULL;

    for (size_t i = 0; i < res->rows; i++)
    {
        for (size_t j = 0; j < res->columns; j++)
        {
            double el = 0;
            for (size_t k = 0; k < A->columns; k++)
                el += mat_el_at(A, i, k) * mat_el_at(B, k, j);
            mat_set_el(res, i, j, el);
        }
    }

    return res;
}

matrix_t *mat_add(matrix_t *A, matrix_t *B)
{
    if (A->columns != B->columns || A->rows != B->rows)
        return NULL;

    matrix_t *res = matrix_create(A->rows, A->columns, 0);
    if (res == NULL)
        return NULL;

    for (size_t i = 0; i < res->rows; i++)
        for (size_t j = 0; j < res->columns; j++)
            mat_set_el(res, i, j, mat_el_at(A, i, j) + mat_el_at(B, i, j));

    return res;
}

matrix_t *mat_transform(matrix_t *A, mat_transform_t transform, int inplace)
{
    matrix_t *res;
    if (inplace)
    {
        res = A;
    }
    else
    {
        res = matrix_create(A->rows, A->columns, 0);
    }

    if (res == NULL)
        return NULL;
    for (size_t i = 0; i < res->rows; i++)
        for (size_t j = 0; j < res->columns; j++)
            mat_set_el(res, i, j, transform(mat_el_at(A, i, j)));
    return res;
}

matrix_t *mat_scalar_multiply(matrix_t *A, double scalar, int inplace)
{
    matrix_t *res;
    if (inplace)
    {
        res = A;
    }
    else
    {
        res = matrix_create(A->rows, A->columns, 0);
    }

    if (res == NULL)
        return NULL;
    for (size_t i = 0; i < res->rows; i++)
        for (size_t j = 0; j < res->columns; j++)
            mat_set_el(res, i, j, mat_el_at(A, i, j) * scalar);
    return res;
}

matrix_t *mat_transpose(matrix_t *A)
{
    matrix_t *res = matrix_create(A->columns, A->rows, 0);
    if (res == NULL)
        return NULL;

    for (size_t i = 0; i < res->rows; i++)
        for (size_t j = 0; j < res->columns; j++)
            mat_set_el(res, i, j, mat_el_at(A, j, i));
    return res;
}

matrix_t *mat_identity(size_t size)
{
    matrix_t *res = matrix_create(size, size, 1);
    while (size--)
        mat_set_el(res, size, size, 1);
    return res;
}

int mat_equal(matrix_t *A, matrix_t *B)
{
    if (A->rows != B->rows || A->columns != B->columns)
        return 0;

    for (size_t i = 0; i < A->rows; i++)
    {
        for (size_t j = 0; j < A->columns; j++)
        {
            if (mat_el_at(A, i, j) != mat_el_at(B, i, j))
            {
                return 0;
            }
        }
    }

    return 1;
}

void mat_fill(matrix_t *mat, double *elements)
{

    for (size_t i = 0; i < mat->rows; i++)
    {
        for (size_t j = 0; j < mat->columns; j++)
        {
            mat_set_el(mat, i, j, elements[i * mat->columns + j]);
        }
    }
}

void mat_print(matrix_t *mat)
{
    for (size_t i = 0; i < mat->rows; i++)
    {
        for (size_t j = 0; j < mat->columns; j++)
        {
            printf("|%3.3f", mat_el_at(mat, i, j));
        }
        printf("|\n");
    }
}

matrix_t *mat_copy(matrix_t *origin)
{
    matrix_t *result = matrix_create(origin->rows, origin->columns, 0);
    if (result == NULL)
        return NULL;
    for (size_t i = 0; i < origin->rows; i++)
    {
        for (size_t j = 0; j < origin->columns; j++)
        {
            mat_set_el(result, i, j, mat_el_at(origin, i, j));
        }
    }

    return result;
}
