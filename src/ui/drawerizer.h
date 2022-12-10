#ifndef UI_DRAWERIZER_H
#define UI_DRAWERIZER_H

#define UNUSED(x) (void)(x)
#define OUTPUT_FOLDER "/tmp/lezcollitade"
#define CELL_SIZE 69
#define BORDER_SIZE 3
#define LINE_THICKNESS 2
#define FONT_SIZE 100
#define FONT_FILE "arial.ttf"

/**
 * \brief Turns a sudoku into a clean grid filled with numbers and saves it
 * \fn void save_sudoku(int grid[])
 * \param grid The given sudoku in the form of an array of int (0 meaning empty
 * cells)
 * \param grid_size The size of the grid (9 or 16)
 */
void save_sudoku(int grid_complete[], int grid[], int grid_size);

/**
 * \brief Draws the lines of the sudoku grid
 * \fn void draw_lines(SDL_Renderer *renderer)
 * \param renderer The renderer where the lines must be drawn
 * \param grid_size The size of the grid (9 or 16)
 */
void draw_lines(SDL_Renderer *renderer, int grid_size);

/**
 * \brief Draws the numbers in the sudoku grid
 * \fn void draw_numbers(SDL_Renderer *renderer, int grid[])
 * \param renderer The renderer where the numbers must be drawn
 * \param grid The given sudoku in the form of an array of int (0 meaning empty
 * cells)
 * \param grid_size The size of the grid (9 or 16)
 */
void draw_numbers(
    SDL_Renderer *renderer, int grid_complete[], int grid[], int grid_size);

#endif
