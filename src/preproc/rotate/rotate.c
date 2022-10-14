#include <err.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <math.h>

static SDL_Surface* load_image(const char* path)
{
    SDL_Surface* temp_surface = IMG_Load(path);
    if (temp_surface == NULL)
        errx(EXIT_FAILURE, "%s", SDL_GetError());

    SDL_Surface* surface =
        SDL_ConvertSurfaceFormat(temp_surface, SDL_PIXELFORMAT_RGB888, 0);

    return surface;
}

SDL_Surface* rotate_surface(SDL_Surface* surface, int deg)
{
    Uint32* pixels = surface->pixels;

    int width = surface->w;
    int height = surface->h;

    int len = width * height;

    deg = deg * M_PI / 180;
    
    int midx = width / 2;
    int midy = height / 2;

    SDL_Surface* new_surface =
        SDL_CreateRGBSurface(0, width, height, 32, 0, 0, 0, 0);

    Uint32* new_pixels = new_surface->pixels;
   
    for (int i = 0; i < len; i++)
    {
        int destx = i % width;
        int desty = i / width;

        int sourcex =
            floor(cos(-deg) * (destx - midx) - sin(-deg) * (desty - midy)) + midx;
        
        int sourcey =
            floor(sin(-deg) * (destx - midx) + cos(-deg) * (desty - midy)) + midy;

        int sourcei = sourcey * width + sourcex;

        if (sourcei >= 0 && sourcei < len)
            new_pixels[i] = pixels[sourcei];
    }

    return new_surface;
}

void rotate_image(const char* file, int deg)
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
        errx(EXIT_FAILURE, "%s", SDL_GetError());

    SDL_Surface* surface = load_image(file);

    SDL_Surface* new_surface = rotate_surface(surface, deg);

    if (IMG_SavePNG(new_surface, "res.png") != 0)
        errx(EXIT_FAILURE, "%s", SDL_GetError());

    SDL_FreeSurface(new_surface);
    SDL_FreeSurface(surface);
    SDL_Quit();
}
