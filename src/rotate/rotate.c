#include <err.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <math.h>

// Updates the display.
//
// renderer: Renderer to draw on.
// texture: Texture that contains the image.
void draw(SDL_Renderer* renderer, SDL_Texture* texture)
{
     if (SDL_RenderCopy(renderer, texture, NULL, NULL) != 0)
        errx(EXIT_FAILURE, "%s", SDL_GetError());

    SDL_RenderPresent(renderer);
}

// Event loop that calls the relevant event handler.
//
// renderer: Renderer to draw on.
// colored: Texture that contains the colored image.
// grayscale: Texture that contains the grayscale image.
void event_loop(SDL_Renderer* renderer, SDL_Texture* texture)
{
    draw(renderer, texture);

    SDL_Event event;

    while (1)
    {
        SDL_WaitEvent(&event);

        switch (event.type)
        {
             case SDL_QUIT:
                return;

            case SDL_WINDOWEVENT:
                draw(renderer, texture);
                break;
        }
    }
}

// Loads an image in a surface.
// The format of the surface is SDL_PIXELFORMAT_RGB888.
//
// path: Path of the image.
SDL_Surface* load_image(const char* path)
{
    SDL_Surface* temp_surface = IMG_Load(path);
    if (temp_surface == NULL)
        errx(EXIT_FAILURE, "%s", SDL_GetError());

    SDL_Surface* surface = SDL_ConvertSurfaceFormat(temp_surface, SDL_PIXELFORMAT_RGB888, 0);

    return surface;
}

int compute_coordinates(int index, int deg, int w)
{
    int x0 = index % w;
    int y0 = index / w;

    double angle = deg * M_PI / 180;

    int x1 = floor(cos(angle) * x0 - sin(angle) * y0) + 200;
    int y1 = floor(sin(angle) * x0 + cos(angle) * y0) + 200;

    int res = y1 * w + x1;

    return res;
}

SDL_Surface* rotate(SDL_Surface* surface)
{
    Uint32* pixels = surface->pixels;

    int len = surface->w * surface->h;

    int size;

    if (surface->w > surface->h)
        size = surface->w;
    else
        size = surface->h;

    SDL_Surface* new_surface = SDL_CreateRGBSurface(0, size, size, 32, 0, 0, 0, 0);

    Uint32* new_pixels = new_surface->pixels;

    for (int i = 0; i < len; i++)
    {
        int new_coords = compute_coordinates(i, 180, surface->w);

        if (new_coords < len && new_coords >= 0)
            new_pixels[new_coords] = pixels[i];
    }

    return new_surface;
}

int main(int argc, char** argv)
{
    // Checks the number of arguments.
    if (argc != 2)
        errx(EXIT_FAILURE, "Usage: image-file");

    if (SDL_Init(SDL_INIT_VIDEO) != 0)
        errx(EXIT_FAILURE, "%s", SDL_GetError());

    SDL_Window* window = SDL_CreateWindow("Rotation testing", 0, 0, 42, 42,
            SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    if (window == NULL)
        errx(EXIT_FAILURE, "%s", SDL_GetError());

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL)
        errx(EXIT_FAILURE, "%s", SDL_GetError());

    SDL_Surface* surface = load_image(argv[1]);

    int w = surface->w;
    int h = surface->h;

    SDL_SetWindowSize(window, w, h);

    SDL_Surface* new_surface = rotate(surface);

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, new_surface);
    if (texture == NULL)
        errx(EXIT_FAILURE, "%s", SDL_GetError());

    SDL_FreeSurface(surface);

    event_loop(renderer, texture);

    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return EXIT_SUCCESS;
}
