#include <err.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <math.h>
#include "../../utils/matrices/matrix.c"

// Updates the display.
//
// renderer: Renderer to draw on.
// texture: Texture that contains the image.
void draw(SDL_Renderer* renderer, SDL_Texture* texture)
{
     // Clears the renderer (sets the background to black).
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    // Sets the color for drawing operations to white.
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);


    if(SDL_RenderCopy(renderer, texture, NULL, NULL) != 0)
        errx(EXIT_FAILURE, "%s", SDL_GetError());

    SDL_RenderPresent(renderer);
}

// Event loop that calls the relevant event handler.
//
// renderer: Renderer to draw on.
// colored: Texture that contains the colored image.
// grayscale: Texture that contains the grayscale image.
void event_loop(SDL_Renderer* renderer, SDL_Texture* colored, SDL_Texture* grayscale)
{
    SDL_Event event;
    SDL_Texture* t = colored;

    while (1)
    {
        SDL_WaitEvent(&event);

        switch (event.type)
        {
            case(SDL_QUIT):
                return;
            case SDL_WINDOWEVENT:
                if (event.window.event == SDL_WINDOWEVENT_RESIZED)
                    draw(renderer, t);
                break;
            case(SDL_KEYDOWN):
                t = t == colored ? grayscale : colored;
                draw(renderer, t);
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
    SDL_Surface* surface = IMG_Load(path);
    if(surface == NULL)
        errx(EXIT_FAILURE, "%s", SDL_GetError());

    surface = SDL_ConvertSurfaceFormat(surface, SDL_PIXELFORMAT_RGB888, 0);

    if(surface == NULL)
        errx(EXIT_FAILURE, "%s", SDL_GetError());
    return surface;
}


void get_max(matrix_t *accumulator)
{
    double max = 0;
    for (size_t i = 0; i < accumulator->rows; i++)
        for (size_t j = 0; j < accumulator->columns; j++)
        {
            double el = mat_el_at(accumulator, i, j);
            if(el > max)
            {
                max = el;
                //printf("\n----------------------------------------------------------------------------------------------------------\n\nmax = %f (%zu, %zu)", max, i, j);
            }
            else
                if(el == max)
                {
                    //printf("max = %f (%zu, %zu)\n", max, i, j);
                }
        }

}

void line_detection(SDL_Surface* surface, matrix_t *accumulator)
{
    SDL_PixelFormat* format = surface->format;
    
    Uint32 *pixels = surface->pixels;
    size_t w = (size_t)surface->w;
    size_t h = (size_t)surface->h;

    SDL_LockSurface(surface);
    for (size_t y = 0; y < h; y++)
    {
        for (size_t x = 0; x < w; x++)
        {
            Uint32 pixel = pixels[y*w+x];
            Uint8 r, g, b;
            SDL_GetRGB(pixel, format, &r, &g, &b);
            
            if (r+g+b == 765) //pixel is white
            {
                for (int t = 0; t < 90; t++)
                {
                    int arg = (t * M_PI) / 180;
                    size_t r = x*cos(arg) + y*sin(arg);
                    mat_set_el(accumulator, r, t, mat_el_at(accumulator, r, t)+1);
                }
            }
        }
    }

    SDL_UnlockSurface(surface);
    get_max(accumulator);
}

int main(int argc, char** argv)
{
    // Checks the number of arguments.
    if (argc != 2)
        errx(EXIT_FAILURE, "Usage: image-file");

    // Initializes the SDL.
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
        errx(EXIT_FAILURE, "%s", SDL_GetError());

    // Creates a window.
    SDL_Window* window = SDL_CreateWindow("Plain Window", 0, 0, 1000, 1000,
            SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    if (window == NULL)
        errx(EXIT_FAILURE, "%s", SDL_GetError());

    // Creates a renderer.
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL)
        errx(EXIT_FAILURE, "%s", SDL_GetError());

    SDL_Surface* surface = load_image(argv[1]);

    //Resizes the window according to the images' size
    SDL_SetWindowSize(window, surface->w, surface->h);

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (texture == NULL)
        errx(EXIT_FAILURE, "%s", SDL_GetError());

    int diag = sqrt(surface->w * surface->w + surface->h * surface->h);
    matrix_t *accumulator = matrix_create(diag, 90, 1);
    line_detection(surface, accumulator);
    matrix_free(accumulator);

    SDL_Texture* texture_gray = SDL_CreateTextureFromSurface(renderer, surface);
    if (texture_gray == NULL)
        errx(EXIT_FAILURE, "%s", SDL_GetError());

    SDL_FreeSurface(surface);

    //Dispatches the events.
    event_loop(renderer, texture, texture_gray);

    // Destroys the objects.
    SDL_DestroyTexture(texture);
    SDL_DestroyTexture(texture_gray);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return EXIT_SUCCESS;
}

