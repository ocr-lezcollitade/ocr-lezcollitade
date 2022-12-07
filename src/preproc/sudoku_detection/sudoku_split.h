#ifndef SUDOKU_SPLIT_H
#define SUDOKU_SPLIT_H
#include "../../utils/matrices/matrix.h"

/**
 * \brief               splits the sudoku grid into 81 squares
 * \fn                  void sudoku_split(char *black_white, char *grayscale,
 * char *path, int dim) \param black_white   the path to the black & white
 * picture of the sudoku \param grayscale     the path to the grayscaled
 * picture of the sudoku \param path          the path where we want to save
 * the images \param dim           the dimension of our sudoku (ex: 9 for the
 * classical one) \return              1 if an error occured, 0 otherwise
 */
int sudoku_split(char *black_white, char *grayscale, char *path, int dim);

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
 * \param dim           the dimension of our sudoku (ex: 9 for the classical
 * one) \return              the angle the image has to be rotated with to be
 * straight if it is found, -1 if not
 */
int get_rotation(SDL_Surface *surface, int dim);

/**
 * \brief               if some lines are too far away or to close to the other
 * lines, they are repositionned (according to the mean of the distances) \fn
 * void calibrate_line(size_t lines[][2], size_t len) \param lines         an
 * array containing the coordinates of all the lines with a similar angle
 * \param len           the length of the array
 */
void calibrate_line(size_t lines[][2], size_t len);

/**
 * \brief               moves the elements from x to the len of the array lines
 * by one to the left \fn                  void move_left(size_t lines[][2],
 * size_t x, size_t len) \param lines         an array containing
 * lines'coordinates \param x             the index from which we start moving
 * everything to the left \param length        the length of the array
 */
void move_left(size_t lines[][2], size_t x, size_t len);

/**
 * \brief               if a line has an angle too different from the mean or
 * if it is too close to other lines, then it is removed. Also harmonizes all
 * thetas \fn                  size_t clean_lines(size_t lines[][2], size_t
 * len, size_t w, size_t h) \param lines         an array containing lines'
 * coordinates \param len           the length of the array \param w the width
 * of the original picture of the sudoku \param h             the height of
 * this same picture \return              the new length of the array
 */
size_t clean_lines(size_t lines[][2], size_t len, size_t w, size_t h);

/**
 * \brief               separates the lines into two sets (vertical and
 * horizontal) then take one of those sets, calculates the mean and returns it
 * (as it is the rotation of the image) \fn                  int
 * rotation(matrix_t *acc, size_t rho, double maximum) \param acc           the
 * accumulator matrix containing all the lines' coordinates \param rho the
 * diagonal of the sudoku picture / the maximum rho of the line \param maximum
 * the maximum value found in the accumulator matrix, used to calculate a ratio
 * and determine which lines are actual lines \return              the average
 * angle by which the image is rotated
 */
int rotation(matrix_t *acc, size_t rho, double maximum, int dim);

#endif
