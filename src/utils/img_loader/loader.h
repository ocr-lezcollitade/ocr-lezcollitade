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
 * \brief Convert a folder of images
 * \fn int convert_multi(char* img_path, char* net_path, char* grid_path,
 * double tres); \param img_path A directory containing all the images \param
 * net_path The path of the trained network \param grid_path The path to save
 * the grids grid.
 * \param grid_elements The number of different elements in the grid
 * (Thus the length of a sudoku line i.e. 9 for regular sudoku).
 * \return int array containing all the detected values, NULL if failed.
 */

int *convert_multi(
    char *img_path, char *net_path, char *grid_path, double tres);

/**
 * \brief Convert a single image to test
 * \fn int convert_single(char* img_path, char* net_path,double tres);
 * \param img_path The path of the image
 * \param net_path The path of the trained network
 * \param mode Display the image and give it's label
 * grid \return 0 if gone well, error otherwise
 */

int convert_single(char *img_path, char *net_path, double tres);

#endif /* !LOADER_H */
