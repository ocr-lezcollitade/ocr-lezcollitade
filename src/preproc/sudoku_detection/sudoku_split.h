#ifndef SUDOKU_SPLIT_H
#define SUDOKU_SPLIT_H

/**
 * \brief               splits the sudoku grid into 81 squares
 * \fn                  void sudoku_split(char *black_white, char *grayscale)
 * \param black_white   the path to the black & white picture of the sudoku
 * \param grayscale     the path to the grayscaled picture of the sudoku
 * \param path          the path where we want to save the images
 */
void sudoku_split(char *black_white, char *grayscale, char *path);

/**
 * \brief               scales down a surface
 * \fn                  SDL_Surface *scale_down(SDL_Surface *surface, size_t w,
 * size_t h, size_t new_w, size_t new_h)
 * \param surface       the surface we want to scale down
 * \param w, h          the width and height of the original surface
 * \param new_w, new_h  the width and height of the surface we want
 */
SDL_Surface *scale_down(
    SDL_Surface *surface, size_t w, size_t h, size_t new_w, size_t new_h);

#endif
