#ifndef PREPROC_COLOR_REMOVAL_COLOR_REMOVAL_H
#define PREPROC_COLOR_REMOVAL_COLOR_REMOVAL_H

/**
 * \brief Turns a surface into grayscale
 * \fn void surface_to_grayscale(SDL_Surface* surface)
 * \param surface The surface to turn into grayscale
 */
void surface_to_grayscale(SDL_Surface *surface);

/**
 * \brief balance the light of the surface according to its most white pixel
 * \fn void white_balance(SDL_Surface *surface)
 * \param surface The surface for which we want to balance the whites
 */
void white_balance(SDL_Surface *surface);

/**
 * \brief Turns a surface into binary
 * \fn void surface_to_binary(SDL_Surface* surface)
 * \param surface The surface to turn into binary
 * \param int The threshold
 */
void surface_to_binary(SDL_Surface *surface, int threshold);

/**
 * \brief Finds the threshold of an image using Otsu's method
 * \fn int otsu(SDL_Surface *surface)
 * \param surface The surface which threshold we want
 * \return The threhsold
 */
int otsu(SDL_Surface *surface);

/**
 * \brief Applies a blur filter to a surface
 * \fn SDL_Surface* blur(SDL_Surface *surface)
 * \param surface The surface which we want to blur
 * \return A new blurred surface
 */
SDL_Surface *blur(SDL_Surface *surface);

/**
 * \brief Turns an image into grayscale
 * \fn void grayscale_image(char *path)
 * \param path The path of the image
 */
void grayscale_image(char *path);

/**
 * \brief Turns an image into binary
 * \fn void binary_image(char *path)
 * \param path The path of the image
 */
void binary_image(char *path);

/**
 * \brief Creates a binary copy of a surface
 *      using gaussian blur and ostu's method
 * \fn SDL_Surface *full_binary(SDL_Surface *surface)
 * \param surface The surface
 * \return The binary surface
 */
SDL_Surface *full_binary(SDL_Surface *surface);

#endif
