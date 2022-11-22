#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <err.h>
#include <malloc.h>

// Updates the display.
//
// renderer: Renderer to draw on.
// texture: Texture that contains the image.
void draw(SDL_Renderer *renderer, SDL_Texture *texture)
{
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);
}

void save_texture(
    const char *file_name, SDL_Renderer *renderer, SDL_Texture *texture)
{
    SDL_Texture *target = SDL_GetRenderTarget(renderer);
    SDL_SetRenderTarget(renderer, texture);
    int width, height;
    SDL_QueryTexture(texture, NULL, NULL, &width, &height);
    SDL_Surface *surface
        = SDL_CreateRGBSurface(0, width, height, 32, 0, 0, 0, 0);
    SDL_RenderReadPixels(renderer, NULL, surface->format->format,
        surface->pixels, surface->pitch);
    IMG_SavePNG(surface, file_name);
    SDL_FreeSurface(surface);
    SDL_SetRenderTarget(renderer, target);
}

void update_renderer(SDL_Renderer *renderer, int x, int y)
{

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    for (int i = -2; i < 3; i++)
    {
        for (int j = -2; j < 3; j++)
        {
            SDL_RenderDrawPoint(renderer, x + i, y + j);
        }
    }
    SDL_RenderPresent(renderer);
}

// Event loop that calls the relevant event handler.
//
// renderer: Renderer to draw on.
// colored: Texture that contains the colored image.
// grayscale: Texture that contains the grayscale image.
void event_loop(
    SDL_Renderer *renderer, SDL_Texture *colored, SDL_Window *window)
{
    SDL_Event event;
    SDL_Texture *t = colored;

    // int* memory = NULL;
    // int memsize=0;

    int clicked = 0;

    while (1)
    {
        SDL_WaitEvent(&event);

        switch (event.type)
        {
            case SDL_QUIT:
                int w, h;
                SDL_GetWindowSize(window, &w, &h);
                t = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB888,
                    SDL_TEXTUREACCESS_STATIC, w, h);
                save_texture("draw.png", renderer, t);
                // free(memory);
                return;

            case SDL_WINDOWEVENT:
                if (event.window.event == SDL_WINDOWEVENT_RESIZED)
                {
                    draw(renderer, t);
                }
                break;

            case SDL_MOUSEBUTTONDOWN:
                switch (event.button.button)
                {
                    case SDL_BUTTON_LEFT:
                        clicked = 1;
                        // memory = realloc(memory,memsize+1);
                        // memory[memsize] = -1;
                        // memsize++;
                        break;
                }
                break;

            case SDL_MOUSEMOTION:
                if (clicked)
                {
                    int x, y;
                    SDL_GetMouseState(&x, &y);
                    update_renderer(renderer, x, y);
                    // memory = realloc(memory,memsize+2);
                    // memory[memsize] = x;
                    // memory[memsize+1] = y;
                    // memsize+=2;
                }
                break;

            case SDL_MOUSEBUTTONUP:
                switch (event.button.button)
                {
                    case SDL_BUTTON_LEFT:
                        clicked = 0;
                        break;
                        /*case SDL_BUTTON_RIGHT:
                           if(memsize>1){

                               int delta = 1;
                               int i = memsize-2;
                               while(memory[i] != -1){
                                   update_renderer_reverse(renderer,memory[i-1],memory[i]);
                                   i-=2;
                                   delta+=2;
                               }
                               memory=realloc(memory,memsize-delta);
                               memsize-=delta;
                           }
                           break;*/
                }
                break;
        }
    }
}

// Loads an image in a surface.
// The format of the surface is SDL_PIXELFORMAT_RGB888.
//
// path: Path of the image.
SDL_Surface *load_image(const char *path)
{
    SDL_Surface *temp = IMG_Load(path);
    if (temp == NULL)
        errx(EXIT_FAILURE, "%s", SDL_GetError());
    SDL_Surface *surface
        = SDL_ConvertSurfaceFormat(temp, SDL_PIXELFORMAT_RGB888, 0);
    if (surface == NULL)
        errx(EXIT_FAILURE, "%s", SDL_GetError());
    SDL_FreeSurface(temp);
    return surface;
}

// Converts a colored pixel into grayscale.
//
// pixel_color: Color of the pixel to convert in the RGB format.
// format: Format of the pixel used by the surface.
Uint32 pixel_to_grayscale(Uint32 pixel_color, SDL_PixelFormat *format)
{
    Uint8 r, g, b;
    SDL_GetRGB(pixel_color, format, &r, &g, &b);
    Uint8 average = 0.3 * r + 0.59 * g + 0.11 * b;
    Uint32 color = SDL_MapRGB(format, average, average, average);
    return color;
}

void surface_to_grayscale(SDL_Surface *surface)
{
    Uint32 *pixels = surface->pixels;
    int len = surface->w * surface->h;
    SDL_PixelFormat *format = surface->format;
    SDL_LockSurface(surface);
    for (int i = 0; i < len; i++)
        pixels[i] = pixel_to_grayscale(pixels[i], format);
    SDL_UnlockSurface(surface);
}

int max(int a, int b)
{
    return a > b ? a : b;
}

int min(int a, int b)
{
    return a < b ? a : b;
}

void update_renderer_reverse(SDL_Renderer *renderer, int x, int y)
{

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    for (int i = -2; i < 3; i++)
    {
        for (int j = -2; j < 3; j++)
        {
            SDL_RenderDrawPoint(renderer, x + i, y + j);
        }
    }
    SDL_RenderPresent(renderer);
}

int main(int argc, char **argv)
{
    // Checks the number of arguments.
    if (argc != 2)
        errx(EXIT_FAILURE, "Usage: image-file");

    // - Initialize the SDL.

    if (SDL_Init(SDL_INIT_VIDEO) != 0)
        errx(EXIT_FAILURE, "%s", SDL_GetError());

    // - Create a window.

    SDL_Window *window = SDL_CreateWindow("Image Grayscale", 0, 0, 600, 800,
        SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    if (window == NULL)
        errx(EXIT_FAILURE, "%s", SDL_GetError());

    // - Create a renderer.

    SDL_Renderer *renderer
        = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL)
        errx(EXIT_FAILURE, "%s", SDL_GetError());

    // - Create a surface from the colored image.

    SDL_Surface *surface = load_image(argv[1]);

    // - Resize the window according to the size of the image.

    int w = surface->w;
    int h = surface->h;

    SDL_SetWindowSize(window, w, h);

    // - Create a texture from the colored surface.

    SDL_Texture *colored = SDL_CreateTextureFromSurface(renderer, surface);

    // - Convert the surface into grayscale.

    // - surface_to_grayscale(surface);

    // - Create a new texture from the grayscale surface.

    // - SDL_Texture* grayscale =
    // SDL_CreateTextureFromSurface(renderer,surface);

    // - Free the surface.

    // - SDL_FreeSurface(surface);

    // - Dispatch the events.

    event_loop(renderer, colored, window);

    // - Destroy the objects.

    SDL_FreeSurface(surface);
    SDL_DestroyTexture(colored);
    // SDL_DestroyTexture(grayscale);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return EXIT_SUCCESS;
}
