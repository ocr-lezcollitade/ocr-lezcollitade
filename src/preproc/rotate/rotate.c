#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <err.h>
#include <math.h>
#include <stdio.h>
#include "rotate.h"
#include "../../utils/img_loader/loader.h"

SDL_Surface *rotate_surface(SDL_Surface *surface, double deg)
{
    SDL_Renderer *renderer = SDL_CreateSoftwareRenderer(surface);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);
    SDL_RenderCopyEx(renderer, texture, NULL, NULL, deg, NULL, SDL_FLIP_NONE);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyTexture(texture);
    return surface;
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
