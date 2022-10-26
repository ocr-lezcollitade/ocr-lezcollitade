#ifndef PREPROC_COLOR_REMOVAL_COLOR_REMOVAL_H
#define PREPROC_COLOR_REMOVAL_COLOR_REMOVAL_H

/**
 * \brief Turns a surface into grayscale
 * \fn void surface_to_grayscale(SDL_Surface* surface)
 * \param surface The surface to turn into grayscale
 */
void surface_to_grayscale(SDL_Surface *surface);

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

#endif
