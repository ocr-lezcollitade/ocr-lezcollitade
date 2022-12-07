#ifndef SOLVER_SOLVER_H
#define SOLVER_SOLVER_H

/**
 * \brief read a sudoku grid from a file of dimension DIM and return int* of
 * the loaded grid \fn int* readGrid(char *  inputFile, int DIM); \param
 * inputFile the name of the input file \param DIM the dimension of the grid
 * (its square root must be an integer and its value <=25) \return NULL if
 * operation failed, int* of the grid otherwise
 */
int *readGrid(char *inputFile, int DIM);

/**
 * \brief write a sudoku grid to a file which path is inputFile.result
 * \fn int* writeGrid(char *  inputFile,int* grid, int DIM);
 * \param inputFile the name of the input file
 * \param grid the int* of the grid
 * \param DIM the dimension of the grid (its square root must be an integer and
 * its value <=25) \return nothing
 */
void writeGrid(char *inputFile, int *grid, int DIM);

/**
 * \brief solve a sudoku grid of dimension DIM (IN PLACE)
 * \fn int* solve(int* grid, int DIM);
 * \param grid the int* of the grid
 * \param DIM the dimension of the grid (its square root must be an integer and
 * its value <=25) \return 1 if solved, 0 otherwise
 */
int solve(int *grid, int DIM);

/**
 * \brief copy a sudoku grid of dimension DIM
 * \fn int* copy_grid(int* grid, int DIM);
 * \param grid the int* of the grid
 * \param DIM the dimension of the grid (its square root must be an integer and
 * its value <=25) \return int* of the copy
 */
int *copy_grid(int *grid, int DIM);

#endif /* !SOLVER_SOLVER_H */
