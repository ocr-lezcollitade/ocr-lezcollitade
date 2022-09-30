#ifndef MATH_MATRIX_H
#define MATH_MATRIX_H

#include <stddef.h>

/**
 *  \struct matrix_t matrix.h
 */
typedef struct
{
    double *_elements; /*! the content of the matrix */
    size_t rows;       /*! the number of rows */
    size_t columns;    /*! the number of columns */
} matrix_t;

/**
 *  \brief          creates a matrix
 *  \fn             matrix_t * matrix_create(size_t rows, size_t columns, int
 * fill) \param rows     the number of rows \param columns  the number of
 * columns \param fill     A flag indicating whether the matrix should be set
 * to 0. \return         the created matrix_t
 */
matrix_t *matrix_create(size_t rows, size_t columns, int fill);

/**
 *  \brief          Creates a matrix and fills it.
 *  \fn             matrix_t *mat_create_fill(size_t rows,
 *                      size_t columns, double *elements)
 *  \param rows     The number of rows.
 *  \param columns  The number of columns.
 *  \param elements The values to fill in.
 *  \return         The created matrix.
 *
 */
matrix_t *mat_create_fill(size_t rows, size_t columns, double *elements);

/**
 *  \brief      frees the matrix
 *  \fn         matrix_free(matrix_t* mat)
 *  \param mat  the matrix to free
 */
void matrix_free(matrix_t *mat);

/**
 *
 * \brief       gets the elements at position i, j
 * \fn          double mat_el_at(matrix_t *mat, size_t i, size_t j);
 * \param mat   The matrix_t to get the element from.
 * \param i     The index of the row.
 * \param j     The index of the column.
 * \return      The value of the element at i, j.
 */
double mat_el_at(matrix_t *mat, size_t i, size_t j);

/**
 *
 * \brief       sets the elements at position i, j
 * \fn          void mat_set_el(matrix_t *mat, size_t i, size_t j, double
 * value); \param mat   The matrix_t to set the element on. \param i     The
 * index of the row. \param j     The index of the column. \param value The
 * value of the element at i, j.
 */
void mat_set_el(matrix_t *mat, size_t i, size_t j, double value);

/**
 *  \brief      Performs the dot product on 2 matrix_t structures
 *  \fn         matrix_t* mat_product(matrix_t *A, matrix_t *B)
 *  \param A    The first matrix to be multiplied.
 *  \param B    The second matrix to be multiplied.
 *  \return     The resutl of the product, NULL if not possible.
 */
matrix_t *mat_product(matrix_t *A, matrix_t *B);

/**
 *  \brief      Performs the addition of 2 matrices.
 *  \fn         matrix_t *mat_add(matrix_t *A, matrix_t *B);
 *  \param A    The first matrix to be added.
 *  \param B    The second matrix to be added.
 *  \return     The result of the addition, NULL if not possible.
 */
matrix_t *mat_add(matrix_t *A, matrix_t *B);

/**
 *  \brief  A function transforming double into new value for each element of
 * the matrix.
 */
typedef double (*mat_transform_t)(double input);

/**
 *  \brief              Applies the transform to each of the matrix_elements
 *  \fn                 matrix_t* mat_transform(matrix_t *A,
 *                          mat_transform_t transform, int inplace)
 *  \param A            The matrix to apply the transform on.
 *  \param transform    The transform to be applied.
 *  \param inplace      A boolean indicating whether the transform should be
 *                      inplace.
 *  \return             A pointer to the transformed matrix.
 */
matrix_t *mat_transform(matrix_t *A, mat_transform_t transform, int inplace);

/**
 *  \brief              Multiplies the matrix by a given scalar.
 *  \fn                 matrix_t *mat_scalar_multiply(
 *                          matrix_t *A,
 *                          double scalar,
 *                          int inplace
 *                      );
 *  \param A            The matrix to apply the scalar on.
 *  \param scalar       The scalar to be applied.
 *  \param inplace      A flag indicating whether the transform should be
 *                          done in place or not.
 *  \return             A pointer to the transform matrix.
 *
 */
matrix_t *mat_scalar_multiply(matrix_t *A, double scalar, int inplace);

/**
 *  \brief          Transposes the given matrix.
 *  \fn             matrix_t *mat_transpose(matrix_t *A);
 *  \param A        The matrix to be transposed.
 *  \return         The transposed matrix.
 */
matrix_t *mat_transpose(matrix_t *A);

/**
 *  \brief          Creates an Identity matrix for given size.
 *  \fn             matrix_t *mat_identity(size_t size);
 *  \param size     The number of rows/cols of the matrix.
 *  \return         The Identity matrix.
 */
matrix_t *mat_identity(size_t size);

/**
 *  \brief          Checks if to matrices are equal.
 *  \fn             int mat_equal(matrix_t *A, matrix_t *B)
 *  \param A        The first member of the equality.
 *  \param B        The second member of the equality.
 *  \return         1 if the matrices are equal, 0 otherwise.
 */
int mat_equal(matrix_t *A, matrix_t *B);

/**
 *  \brief          Fills the matrix with the given elements.
 *  \fn             void mat_fill(matrix_t *mat, double *elements)
 *  \param mat      The matrix to be filled.
 *  \param elements The array to fill the matrix with. It is assumed to have
 *                  the correct dimensions (rows * columns).
 */
void mat_fill(matrix_t *mat, double *elements);

/**
 * \brief           Prints the matrix.
 * \fn              void mat_print(matrix_t *mat)
 * \param mat       The matrix to print.
 */
void mat_print(matrix_t *mat);

/**
 *  \brief          Copies the matrix.
 *  \fn             matrix_t *mat_copy(matrix_t *origin);
 *  \param origin   The origin matrix.
 *  \return         A copy of the matrix.
 */
matrix_t *mat_copy(matrix_t *origin);

#endif /* !MATH_MATRIX_H */
