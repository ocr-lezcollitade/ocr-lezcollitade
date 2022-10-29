#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <err.h>
#include <math.h>
#include <stdio.h>
#include "rotate.h"
#include "../../utils/img_loader/loader.h"

SDL_Surface *rotate_surface(SDL_Surface *surface, double deg)
{
    Uint32 *pixels = surface->pixels;

    int width = surface->w;
    int height = surface->h;

    int len = width * height;

    deg = deg * M_PI / 180;

    int midx = width / 2;
    int midy = height / 2;

    SDL_Surface *new_surface
        = SDL_CreateRGBSurface(0, width, height, 32, 0, 0, 0, 0);

    Uint32 *new_pixels = new_surface->pixels;

    for (int i = 0; i < len; i++)
    {
        int destx = i % width;
        int desty = i / width;

        int sourcex
            = floor(cos(-deg) * (destx - midx) - sin(-deg) * (desty - midy))
              + midx;

        int sourcey
            = floor(sin(-deg) * (destx - midx) + cos(-deg) * (desty - midy))
              + midy;

        int sourcei = sourcey * width + sourcex;

        if (sourcei >= 0 && sourcei < len)
            new_pixels[i] = pixels[sourcei];
        else
            new_pixels[i] = SDL_MapRGB(surface->format, 255, 255, 255);
    }

    return new_surface;
}

void rotate_image(const char *file, double deg)
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
        errx(EXIT_FAILURE, "%s", SDL_GetError());

    SDL_Surface *surface = load_image(file);

    SDL_Surface *new_surface = rotate_surface(surface, deg);

    if (IMG_SavePNG(new_surface, "rotated.png") != 0)
        errx(EXIT_FAILURE, "%s", SDL_GetError());

    SDL_FreeSurface(new_surface);
    SDL_FreeSurface(surface);
    SDL_Quit();
}
