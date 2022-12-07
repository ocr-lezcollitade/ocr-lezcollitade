#ifndef LOADER_H
#define LOADER_H
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "../../network/network.h"

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

double *surface_to_grayscale_img(size_t nb, SDL_Surface *surface);

/**
 * \brief Convert a folder of images into an int* of the grid
 * \fn int convert_multi(char* img_path, network_t* net,
 * double tres);
 * \param img_path A directory containing all the images
 * \param net The loaded trained network
 * \param tres the treshold for the network
 * \return int array containing all the detected values, NULL if failed.
 */

int *convert_multi(char *img_path, network_t *net, double tres);

/**
 * \brief Test a single image
 * \fn int test_single(network_t* net,char* img_path,double tres);
 * \param img_path The path of the image
 * \param net The loaded trained network
 * \param tres The treshold for the network
 * \return 0 if gone well, error otherwise
 */

int test_single(network_t *net, char *img_path, double tres);

/**
 * \brief Convert a matrix of pixel values into the corresponding number
 * \fn int convert_single(network_t* net,double* values,double tres);
 * \param values The matrix of pixel
 * \param net The loaded trained network
 * \param tres The treshold for the network
 * \return the predicted number given by the network
 */

int convert_single(network_t *net, double *values, double tres);

#endif /* !LOADER_H */
