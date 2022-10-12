#ifndef ROTATE_ROTATE_H
#define ROTATE_ROTATE_H

/**
 * \brief Applies a given rotation to a given surface
 * \fn SDL_Surface* rotate_surface(SDL_Surface* surface, int deg);
 * \param surface; the surface to rotate
 * \param deg; the angle of the rotation
 * \return the a new rotated surface
 */
SDL_Surface* rotate_surface(SDL_Surface* surface, int deg);

/**
 * \brief Applies a given rotation to a image
 * \fn void rotate_image(const char* file, int deg);
 * \param file; the path to the file to the rotate
 * \param deg; the angle of the rotation
 * \return void
 */
void rotate_image(const char* file, int deg);
