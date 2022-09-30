#include <criterion/criterion.h>
#include <criterion/new/assert.h>
#include <criterion/parameterized.h>
#include <stdio.h>
#include "../../../src/utils/matrices/matrix.h"

TestSuite(matrix_create);

Test(matrix_create, simple_test)
{
    matrix_t *mat = matrix_create(3, 4, 1);
    cr_assert(eq(ulong, 3, mat->rows));
    cr_expect(mat->columns == 4, "The number of columns should be 4.");
    for (size_t i = 0; i < mat->rows; i++)
        for (size_t j = 0; j < mat->columns; j++)
            cr_expect(mat_el_at(mat, i, j) == 0, "The elements should be 0");

    double val = (double)0x12;
    mat_set_el(mat, 2, 1, val);
    cr_expect(mat_el_at(mat, 2, 1) == val, "mat_set_el should set the value");

    double values[4] = {1, 2, 3, 4};
    matrix_free(mat);
    mat = matrix_create(2, 2, 0);
    mat_fill(mat, (double *)values);
}

TestSuite(matrix_add);

Test(matrix_add, basics)
{
    matrix_t *A = matrix_create(3, 4, 1);
    matrix_t *B = matrix_create(3, 4, 1);

    for (size_t i = 0; i < A->rows; i++)
    {
        for (size_t j = 0; j < A->columns; j++)
        {
            mat_set_el(A, i, j, i);
            mat_set_el(B, i, j, j);
        }
    }
    matrix_t *C = mat_add(A, B);
    matrix_free(A);
    matrix_free(B);
    for (size_t i = 0; i < C->rows; i++)
    {
        for (size_t j = 0; j < C->columns; j++)
        {
            cr_expect(mat_el_at(C, i, j) == i + j,
                "The elements be the sum of A + B");
        }
    }
    matrix_free(C);
}

TestSuite(matrix_product);

Test(matrix_product, basics)
{
    double v1[9] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    matrix_t *A = matrix_create(3, 3, 0);
    mat_fill(A, (double *)v1);
    matrix_t *B = mat_identity(3);
    matrix_t *res = mat_product(A, B);
    matrix_t *expected = A;
    cr_expect(mat_equal(res, expected), "res should be equal to expected");

    res = mat_product(B, A);
    cr_expect(mat_equal(res, expected), "res should be equal to expected");
}

Test(matrix_product, vectors)
{
    matrix_t *A = matrix_create(1, 2, 0);
    double v1[] = {1, 2};
    mat_fill(A, (double *)v1);
    double v2[] = {3, 4};
    matrix_t *B = matrix_create(2, 1, 0);
    mat_fill(B, (double *)v2);
    matrix_t *expected = matrix_create(1, 1, 0);
    mat_set_el(expected, 0, 0, 11);

    matrix_t *res = mat_product(A, B);

    cr_expect(mat_equal(res, expected), "vector prod should be equal");
    matrix_free(expected);
    matrix_free(res);
    res = mat_product(B, A);
    expected = matrix_create(2, 2, 0);
    double v3[] = {3, 6, 4, 8};
    mat_fill(expected, v3);

    cr_expect(mat_equal(res, expected), "vector prod should be equal");
}
