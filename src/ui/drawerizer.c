#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <gtk/gtk.h>
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "drawerizer.h"
#include "../preproc/color_removal/color_removal.h"
#include "../preproc/rotate/rotate.h"
#include "../preproc/sudoku_detection/sudoku_split.h"
#include "../utils/img_loader/loader.h"
#include "ui.h"

void save_sudoku(int grid_complete[], int grid[], int grid_size)
{
    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_PNG);

    SDL_Surface *base_surface
        = SDL_CreateRGBSurface(0, grid_size * CELL_SIZE + BORDER_SIZE * 2,
            grid_size * CELL_SIZE + BORDER_SIZE * 2, 32, 0, 0, 0, 0);
    SDL_Renderer *renderer = SDL_CreateSoftwareRenderer(base_surface);

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);

    draw_lines(renderer, grid_size);
    draw_numbers(renderer, grid_complete, grid, grid_size);

    SDL_Surface *surface
        = SDL_CreateRGBSurface(0, grid_size * CELL_SIZE + BORDER_SIZE * 2,
            grid_size * CELL_SIZE + BORDER_SIZE * 2, 32, 0, 0, 0, 0);
    SDL_RenderReadPixels(renderer, NULL, SDL_PIXELFORMAT_ARGB8888,
        surface->pixels, surface->pitch);

    char filename[50];
    snprintf(filename, 50, "%s/completed.png", OUTPUT_FOLDER);
    IMG_SavePNG(surface, filename);

    SDL_FreeSurface(surface);
    SDL_DestroyRenderer(renderer);
    SDL_FreeSurface(base_surface);
    IMG_Quit();
    SDL_Quit();
}

void draw_lines(SDL_Renderer *renderer, int grid_size)
{
    for (int i = 0; i < grid_size + 1; i++)
    {
        SDL_RenderDrawLine(renderer, BORDER_SIZE, BORDER_SIZE + i * CELL_SIZE,
            grid_size * CELL_SIZE + BORDER_SIZE, BORDER_SIZE + i * CELL_SIZE);
        SDL_RenderDrawLine(renderer, BORDER_SIZE + i * CELL_SIZE, BORDER_SIZE,
            BORDER_SIZE + i * CELL_SIZE, grid_size * CELL_SIZE + BORDER_SIZE);
    }

    SDL_SetRenderDrawColor(renderer, 255, 0, 0, SDL_ALPHA_OPAQUE);

    int step = 3;
    if (grid_size == 16)
        step = 4;

    for (int i = 0; i < grid_size + 1; i += step)
    {
        for (int j = 0; j < LINE_THICKNESS; j++)
            SDL_RenderDrawLine(renderer, BORDER_SIZE,
                BORDER_SIZE + i * CELL_SIZE + j,
                grid_size * CELL_SIZE + BORDER_SIZE,
                BORDER_SIZE + i * CELL_SIZE + j);
        for (int j = 0; j < LINE_THICKNESS; j++)
            SDL_RenderDrawLine(renderer, BORDER_SIZE + i * CELL_SIZE + j,
                BORDER_SIZE, BORDER_SIZE + i * CELL_SIZE + j,
                grid_size * CELL_SIZE + BORDER_SIZE);
    }
}

void draw_numbers(
    SDL_Renderer *renderer, int grid_complete[], int grid[], int grid_size)
{
    TTF_Init();
    TTF_Font *font = TTF_OpenFont(FONT_FILE, FONT_SIZE);

    for (int i = 0; i < grid_size; i++)
    {
        for (int j = 0; j < grid_size; j++)
        {
            SDL_Color textColor = {0, 0, 0, SDL_ALPHA_OPAQUE};

            if (grid[i * grid_size + j] == 0)
            {
                textColor.r = 0;
                textColor.g = 255;
                textColor.b = 0;
            }
            else
            {
                textColor.r = 0;
                textColor.g = 0;
                textColor.b = 0;
            }

            char text[5];
            if (grid[i * grid_size + j] >= 10)
                snprintf(text, 5, "%c", grid_complete[i * grid_size + j] + 55);
            else
                snprintf(text, 5, "%i", grid_complete[i * grid_size + j]);

            SDL_Surface *surface = TTF_RenderText_Solid(font, text, textColor);
            SDL_Texture *texture
                = SDL_CreateTextureFromSurface(renderer, surface);

            SDL_Rect dstRect;
            dstRect.w = CELL_SIZE / 2;
            dstRect.h = CELL_SIZE / 2;
            dstRect.x
                = BORDER_SIZE + j * CELL_SIZE + (CELL_SIZE - dstRect.w) / 2;
            dstRect.y
                = BORDER_SIZE + i * CELL_SIZE + (CELL_SIZE - dstRect.h) / 2;

            SDL_RenderCopy(renderer, texture, NULL, &dstRect);

            SDL_DestroyTexture(texture);
            SDL_FreeSurface(surface);
        }
    }

    TTF_CloseFont(font);
    TTF_Quit();
}
