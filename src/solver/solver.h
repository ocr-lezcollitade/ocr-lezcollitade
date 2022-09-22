#ifndef SOLVER_SOLVER_H
#define SOLVER_SOLVER_H


/** 
* \brief Checks if all the rows are valid
* \fn int checkRow(int grid[9][9])
* \param grid 2-d array represting the sudoku
* \return 1 if the rows are valids 0 otherwise
*/
int checkRow(int grid[9][9]);

/** 
* \brief Checks if all the columns are valid
* \fn int checkCol(int grid[9][9])
* \param grid 2-d array represting the sudoku
* \return 1 if all the columns are valid 0 otherwise
*/
int checkCol(int grid[9][9]);

/** 
* \brief Checks if all the squares are valid
* \fn int checkSquare(int grid[9][9])
* \param grid 2-d array represting the sudoku
* \return 1 if all the squares are valid 0 otherwise
*/
int checkSquare(int grid[9][9]);

/** 
* \brief Checks if all the rows, columns and squares are valid
* \fn int allChecks(int grid[9][9])
* \param grid 2-d array represting the sudoku
* \return 1 if all the rows, columns and squares are valid 0 otherwise
*/
int allChecks(int grid[9][9]);

/** 
* \brief Checks if all the cases have a number and all the rows, columns and squares are valid
* \fn int win(int grid[9][9])
* \param grid 2-d array represting the sudoku
* \return 1 if all the cases have a number and all the rows, columns and squares are valid 0 otherwise
*/
int win(int grid[9][9]);

/** 
* \brief read a sudoku grid from a file, solve it and write the solution in another file in ".result"
* \fn int solve(char *  inputFile);
* \param the name og the input file
* \return 0 if the grid is solvable, 0 otherwise
*/
int solve(char *  inputFile);

#endif /* !SOLVER_SOLVER_H */
