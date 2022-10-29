#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <err.h>
#include "color_removal.h"
#include "../../utils/img_loader/loader.h"

static Uint32 pixel_to_gray(Uint32 pixel_color, SDL_Surface *surface)
{
    Uint8 r, g, b;
    SDL_GetRGB(pixel_color, surface->format, &r, &g, &b);

    Uint8 average = floor(0.299 * r + 0.587 * g + 0.114 * b);

    r = 255 - average;
    g = 255 - average;
    b = 255 - average;

    Uint32 color = SDL_MapRGB(surface->format, r, g, b);

    return color;
}

void surface_to_grayscale(SDL_Surface *surface)
{
    Uint32 *pixels = surface->pixels;

    for (int i = 0; i < (surface->w * surface->h); i++)
        pixels[i] = pixel_to_gray(pixels[i], surface);
}

static Uint32 pixel_to_binary(
    Uint32 pixel_color, SDL_Surface *surface, int threshold)
{
    Uint8 r, g, b;
    SDL_GetRGB(pixel_color, surface->format, &r, &g, &b);

    Uint8 average = 0.299 * r + 0.587 * g + 0.114 * b;

    if (average > threshold)
        r = g = b = 0;
    else
        r = g = b = 255;

    Uint32 color = SDL_MapRGB(surface->format, r, g, b);

    return color;
}

void surface_to_binary(SDL_Surface *surface, int threshold)
{
    Uint32 *pixels = surface->pixels;

    for (int i = 0; i < (surface->w * surface->h); i++)
        pixels[i] = pixel_to_binary(pixels[i], surface, threshold);
}

/*
void print_histo(int histo[])
{
    for (int i = 0; i < 256; i++)
    {
        int n = histo[i];
        printf("%i ", i);
        for (int j = 0; j < n/42; j++)
            printf("*");
        printf("\n");
    }
}
*/

static void fill_histo(SDL_Surface *surface, int histo[])
{
    Uint32 *pixels = surface->pixels;

    Uint8 r, g, b;

    int length = surface->w * surface->h;
    for (int i = 0; i < length; i++)
    {
        SDL_GetRGB(pixels[i], surface->format, &r, &g, &b);

        int gray = floor(0.299 * r + 0.587 * g + 0.114 * b);

        histo[gray] = histo[gray] + 1;
    }
}

int otsu(SDL_Surface *surface)
{
    int histo[256] = {0};
    fill_histo(surface, histo);

    int total = surface->w * surface->h;
    int level = 0;
    double sumB = 0, max = 0, sum = 0, wB = 0;
    double wF, mean_F;

    for (int i = 0; i < 256; i++)
        sum += i * histo[i];

    for (int i = 1; i < 200; i++)
    {
        wF = total - wB;
        if (wB > 0 && wF > 0)
        {
            mean_F = (sum - sumB) / wF;
            double val
                = wB * wF * ((sumB / wB) - mean_F) * ((sumB / wB) - mean_F);
            if (val >= max)
            {
                level = i;
                max = val;
            }
        }
        wB += histo[i];
        sumB = sumB + (i - 1) * histo[i];
    }

    return level;
}

/*
    median 3x3
    int kernel[] =
        {1, 1, 1,
         1, 1, 1,
         1, 1, 1};

    gaussian 3x3
    int kernel[] =
        {1, 2, 1,
         2, 4, 2,
         1, 2, 1};

    gaussian 5x5
    int kernel[] =
        {1, 4, 7, 4, 1,
         4, 16, 26, 16, 4,
         7, 26, 41, 26, 7,
         4, 16, 26, 16, 4,
         1, 4, 7, 4, 1};
*/
SDL_Surface *blur(SDL_Surface *surface)
{
    int kernel[] = {1, 2, 1, 2, 4, 2, 1, 2, 1};
    int size = 3;

    SDL_Surface *new_surface
        = SDL_CreateRGBSurface(0, surface->w, surface->h, 32, 0, 0, 0, 0);

    Uint32 *pixels = surface->pixels;
    Uint32 *new_pixels = new_surface->pixels;
    Uint8 r, g, b;

    for (int x = 0; x < surface->w; x++)
        for (int y = 0; y < surface->h; y++)
        {
            int shift = size / 2;
            int weight = 0, sum = 0, ki = 0;

            for (int s_x = shift - size; s_x < shift; s_x++)
                for (int s_y = shift - size; s_y < shift; s_y++)
                {
                    int coord = (y + s_y) * surface->w + x + s_x;
                    if (coord >= 0)
                    {
                        SDL_GetRGB(pixels[coord], surface->format, &r, &g, &b);

                        sum += kernel[ki]
                               * floor(0.299 * r + 0.587 * g + 0.114 * b);
                        weight += kernel[ki];
                    }
                    ki++;
                }

            int coord = y * surface->w + x;
            int c = sum / weight;

            Uint32 color = SDL_MapRGB(surface->format, c, c, c);
            new_pixels[coord] = color;
        }

    return new_surface;
}

SDL_Surface *full_binary(SDL_Surface *surface)
{
    surface_to_grayscale(surface);

    int threshold = otsu(surface);

    SDL_Surface *blurry = blur(surface);

    surface_to_binary(blurry, threshold);

    return blurry;
}

void grayscale_image(char *path)
{
    SDL_Surface *surface = load_image(path);

    surface_to_grayscale(surface);

    IMG_SavePNG(surface, "grayscaled.png");

    SDL_FreeSurface(surface);
}

void binary_image(char *path)
{
    SDL_Surface *surface = load_image(path);

    surface_to_grayscale(surface);

    int threshold = otsu(surface);

    SDL_Surface *blurry = blur(surface);

    surface_to_binary(blurry, threshold);

    IMG_SavePNG(blurry, "binarized.png");

    SDL_FreeSurface(surface);
    SDL_FreeSurface(blurry);
}
