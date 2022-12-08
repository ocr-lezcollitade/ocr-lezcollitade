#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <err.h>
#include <math.h>
#include <stdio.h>
#include "crop.h"
#include "../../network/network.h"
#include "../../utils/img_loader/loader.h"

SDL_Surface *crop_surface(
    SDL_Surface *surface, float left, float top, float right, float bottom)
{

    if (left + right > 1)
    {
        right = 0;
        left = 1;
    }

    if (top + bottom > 1)
    {
        top = 0;
        bottom = 1;
    }

    SDL_Rect positionI;

    positionI.x = (int)((float)surface->w * left);
    positionI.y = (int)((float)surface->h * top);
    ;

    positionI.h = (int)((float)surface->h * (1 - bottom)) - positionI.y;
    positionI.w = (int)((float)surface->w * (1 - right)) - positionI.x;

    SDL_Surface *res
        = SDL_CreateRGBSurface(0, positionI.w, positionI.h, 32, 0, 0, 0, 0);

    SDL_BlitSurface(surface, &positionI, res, NULL);

    return res;
}

void crop_image(const char *file, float left, float top, float right,
    float bottom, const char *output)
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
        errx(EXIT_FAILURE, "%s", SDL_GetError());

    SDL_Surface *surface = load_image(file);

    SDL_Surface *new_surface = crop_surface(surface, left, top, right, bottom);

    if (IMG_SavePNG(new_surface, output) != 0)
        errx(EXIT_FAILURE, "%s", SDL_GetError());

    SDL_FreeSurface(new_surface);
    SDL_FreeSurface(surface);
    SDL_Quit();
}
