#ifndef LOADER_H
#define LOADER_H
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#define SINGLE_IMAGE 1
#define MULTI_IMAGE SINGLE_IMAGE + 1
/**
 * \brief Loads an image into a surface with SDL
 * \fn SDL_Surface* load_image(const char* path)
 * \param path The path of the image
 * \return The surface corresponding to the image
 */

SDL_Surface *load_image(const char *path);

/**
 * \brief Convert a pixel into a grayscale value and stores it into a list
 * \fn pixel_to_grayscale(Uint32 pixel_color, SDL_PixelFormat* format, double*
 * values, size_t i); \param pixel_color The rgb value of the pixel \param
 * format The format of the image \param values The array to stores the pixels
 * \param i The index of the pixel
 */

void pixel_to_grayscale(
    Uint32 pixel_color, SDL_PixelFormat *format, double *values, size_t i);

/**
 * \brief Convert a surface into grayscale an return all the values of each
 * pixels \fn double* surface_to_grayscale_img(SDL_Surface* surface); \param
 * surface The image surface \return An double arrays of the values in
 * grayscale of each pixels
 */

double *surface_to_grayscale_img(SDL_Surface *surface);

/**
 * \brief Convert images according to the mode
 * \fn int convert(char* img_path, char* net_path, char* grid_path, int mode);
 * \param img_path => If mode 1 : The path of the image
 * \param img_path => If mode 2 : A directory containing all the images
 * \param net_path The path of the trained network
 * \param grid_path => Only in mode 2 : The path to save the grids
 * \param mode => If 1 : Display the image and give it's label
 * \param mode => If 2 : Takes a directory of image and convert them into a
 * grid \return 0 if gone well, error otherwise
 */

int convert(char *img_path, char *net_path, char *grid_path, int mode);

#endif /* !LOADER_H */
