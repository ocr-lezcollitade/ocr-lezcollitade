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

    SDL_Surface* surface = SDL_ConvertSurfaceFormat(temp_surface, SDL_PIXELFORMAT_RGB888, 0);

    return surface;
}

SDL_Surface* rotate_surface(SDL_Surface* surface, int deg)
{
    Uint32* pixels = surface->pixels;

    int w = surface->w;
    int h = surface->h;

    int len = surface->w * surface->h;

    double angle = deg * M_PI / 180;
    
    int midx = w / 2;
    int midy = h / 2;

    SDL_Surface* new_surface = SDL_CreateRGBSurface(0, w, h, 32, 0, 0, 0, 0);

    Uint32* new_pixels = new_surface->pixels;
   
    for (int i = 0; i < len; i++)
    {
        int x_d = i % w;
        int y_d = i / w;

        int x_s = floor(cos(-angle) * (x_d - midx) - sin(-angle) * (y_d - midy)) + midx;
        int y_s = floor(sin(-angle) * (x_d - midx) + cos(-angle) * (y_d - midy)) + midy;

        int i_s = y_s * w + x_s;

        if (i_s >= 0 && i_s < len)
            new_pixels[i] = pixels[i_s];
    }

    return new_surface;
}

void rotate_image(const char* file, int deg)
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
        errx(EXIT_FAILURE, "%s", SDL_GetError());

    SDL_Surface* surface = load_image(file);

    SDL_Surface* new_surface = rotate_surface(surface, deg);

    if (SDL_SaveBMP(new_surface, "rotated.bmp") != 0)
        errx(EXIT_FAILURE, "%s", SDL_GetError());

    SDL_FreeSurface(new_surface);
    SDL_FreeSurface(surface);
    SDL_Quit();
}

int main(int argc, char** argv)
{
    if (argc != 3)
        errx(EXIT_FAILURE, "Usage: ./rotate <file> <angle>");

    if (SDL_Init(SDL_INIT_VIDEO) != 0)
        errx(EXIT_FAILURE, "%s", SDL_GetError());

    SDL_Surface* surface = load_image(argv[1]);

    int angle = atoi(argv[2]);

    SDL_Surface* new_surface = rotate_surface(surface, angle);

    if (SDL_SaveBMP(new_surface, "rotated.bmp") != 0)
        errx(EXIT_FAILURE, "%s", SDL_GetError());

    SDL_FreeSurface(new_surface);
    SDL_FreeSurface(surface);

    SDL_Quit();

    return EXIT_SUCCESS;
}
