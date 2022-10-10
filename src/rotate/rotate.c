#include <err.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <math.h>

void draw(SDL_Renderer* renderer, SDL_Texture* texture)
{
     if (SDL_RenderCopy(renderer, texture, NULL, NULL) != 0)
        errx(EXIT_FAILURE, "%s", SDL_GetError());

    SDL_RenderPresent(renderer);
}

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

SDL_Surface* load_image(const char* path)
{
    SDL_Surface* temp_surface = IMG_Load(path);
    if (temp_surface == NULL)
        errx(EXIT_FAILURE, "%s", SDL_GetError());

    SDL_Surface* surface = SDL_ConvertSurfaceFormat(temp_surface, SDL_PIXELFORMAT_RGB888, 0);

    return surface;
}

SDL_Surface* rotate_from_dest(SDL_Surface* surface, int deg)
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

int main(int argc, char** argv)
{
    // Checks the number of arguments.
    if (argc != 3)
        errx(EXIT_FAILURE, "Usage: ./rotate <file> <angle>");

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

    SDL_Surface* new_surface = rotate_from_dest(surface, (argv[2][0] - '0') * 10 + argv[2][1] - '0');

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, new_surface);
    if (texture == NULL)
        errx(EXIT_FAILURE, "%s", SDL_GetError());

    if (SDL_SaveBMP(new_surface, "rotated.bmp") != 0)
        errx(EXIT_FAILURE, "%s", SDL_GetError());

    SDL_FreeSurface(new_surface);
    SDL_FreeSurface(surface);

    event_loop(renderer, texture);

    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return EXIT_SUCCESS;
}
