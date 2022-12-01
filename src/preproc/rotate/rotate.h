#ifndef PREPROC_ROTATE_ROTATE_H
#define PREPROC_ROTATE_ROTATE_H

/**
 * \brief Creates a rotated clone of a given surface, freeing the latter
 * \fn SDL_Surface* rotate_surface(SDL_Surface* surface, int deg)
 * \param surface The surface to rotate
 * \param deg The angle of the rotation
 * \return A new rotated surface
 */
SDL_Surface *rotate_surface(SDL_Surface *surface, double deg);

/**
 * \brief Creates a rotated clone (res.png) of a given image
 * \fn void rotate_image(const char* file, int deg)
 * \param file The path to the image to the rotate
 * \param deg The angle of the rotation
 */
void rotate_image(const char *file, double deg);

#endif
