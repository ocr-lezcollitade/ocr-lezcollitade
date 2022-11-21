#ifndef SUDOKU_SPLIT_H
#define SUDOKU_SPLIT_H

/**
 * \brief               splits the sudoku grid into 81 squares
 * \fn                  void sudoku_split(char *black_white, char *grayscale)
 * \param black_white   the path to the black & white picture of the sudoku
 * \param grayscale     the path to the grayscaled picture of the sudoku
 * \param path          the path where we want to save the images
 * \return              1 if an error occured, 0 otherwise
 */
int sudoku_split(char *black_white, char *grayscale, char *path);

/**
 * \brief               scales down a surface
 * \fn                  void scale_down(SDL_Surface *surface, size_t w,
 * size_t h, size_t new_w, size_t new_h)
 * \param surface       the surface we want to scale down
 * \param w, h          the width and height of the original surface
 * \param new_w, new_h  the width and height of the surface we want
 */
void scale_down(
    SDL_Surface **surface, size_t w, size_t h, size_t new_w, size_t new_h);

/**
 * \brief               finds the angle the image has to be rotated with to be
 * straight \fn                  ssize_t get_rotation(SDL_Surface *surface)
 * \param surface       the surface we went the rotation of
 * \return              the angle the image has to be rotated with to be
 * straight if it is found, -1 if not
 */
int get_rotation(SDL_Surface *surface);

#endif
