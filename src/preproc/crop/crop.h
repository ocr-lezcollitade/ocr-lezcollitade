#ifndef PREPROC_CROP_CROP_H
#define PREPROC_CROP_CROP_H

/**
 * \brief Creates a croped clone of a given surface, freeing the latter
 * \fn SDL_Surface* crop_surface(SDL_Surface* surface, float left, float, top,
 * float right, float bottom); \param surface The surface to crop \param left
 * The amount to crop on left part ( [0,1] ) \param right The amount to crop on
 * right part ( [0,1] ) \param top The amount to crop on top part ( [0,1] )
 * \param bottom The amount to crop on bottom part ( [0,1] )
 * \return A new rotated surface
 */
SDL_Surface *crop_surface(
    SDL_Surface *surface, float left, float top, float right, float bottom);

/**
 * \brief Creates a cropped clone (res.png) of a given image
 * \fn void crop_image(const char* file, float left, float top, float right,
 * float bottom) \param file The path to the image to crop \param left The
 * amount to crop on left part ( [0,1] ) \param right The amount to crop on
 * right part ( [0,1] ) \param top The amount to crop on top part ( [0,1] )
 * \param bottom The amount to crop on bottom part ( [0,1] )
 * \param output The path to the output file.
 *
 */
void crop_image(const char *file, float left, float top, float right,
    float bottom, const char *ouput);

#endif
