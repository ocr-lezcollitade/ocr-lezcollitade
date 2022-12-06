#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <dirent.h>
#include <err.h>
#include <math.h>
#include "loader.h"
#include "../../network/network.h"
#include "../../solver/solver.h"
#include "../matrices/matrix.h"

SDL_Surface *load_image(const char *path)
{
    SDL_Surface *temp = IMG_Load(path);
    SDL_Surface *res
        = SDL_ConvertSurfaceFormat(temp, SDL_PIXELFORMAT_RGB888, 0);
    SDL_FreeSurface(temp);
    return res;
}
void pixel_to_grayscale(
    Uint32 pixel_color, SDL_PixelFormat *format, double *values, size_t i)
{
    Uint8 r, g, b;
    SDL_GetRGB(pixel_color, format, &r, &g, &b);
    Uint8 average = 0.3 * r + 0.59 * g + 0.11 * b;
    double val = (double)average / 255.0;
    values[i] = val >= 0.5 ? val : 0.0;
}

double *surface_to_grayscale_img(SDL_Surface *surface)
{
    double *values = malloc(sizeof(double) * 784);
    Uint32 *pixels = surface->pixels;
    SDL_PixelFormat *format = surface->format;

    for (size_t i = 0; i < 784; i++)
    {
        if (SDL_LockSurface(surface) != 0)
            errx(EXIT_FAILURE, "%s", SDL_GetError());
        pixel_to_grayscale(pixels[i], format, values, i);
        SDL_UnlockSurface(surface);
    }
    return values;
}

static void print(double *values)
{

    for (size_t i = 0; i < 28; i++)
    {
        for (size_t j = 0; j < 28; j++)
        {
            if (values[i * 28 + j] != 0.0)
                printf("██");
            else
                printf("  ");
        }
        printf("\n");
    }
}

static int get_index(char *name)
{
    char *temp = malloc(sizeof(char) * 2);
    int i = 0;
    while (i < 2 && name[i] != '.')
    {
        temp[i] = name[i];
        i++;
    }
    if (i >= 3)
        errx(1, "get_index: name overflowing buffer");
    temp[i] = 0;
    int val = atoi(temp);
    free(temp);
    return val;
}

int convert_single(char *img_path, char *net_path, double tres)
{

    if (SDL_Init(SDL_INIT_VIDEO) != 0)
        errx(EXIT_FAILURE, "%s", SDL_GetError());

    SDL_Surface *surface = load_image(img_path);

    double *values = surface_to_grayscale_img(surface);
    SDL_FreeSurface(surface);

    matrix_t *pixels = mat_create_fill(784, 1, values);
    network_t *net = network_load(net_path);
    matrix_t *res = compute_results(pixels, net);
    int out = network_get_output(res, tres);

    double sum = 0;
    for (size_t k = 0; k < net->inputs; k++)
        sum += values[k];

    if (sum == 0)
        out = 0;

    print(values);

    printf("It's a ");
    if (out < 0)
        printf("%d", out);
    else if (out < 10)
        printf("%c", out + '0');
    else
        printf("%c", out - 10 + 'A');
    printf("!\n");
    matrix_free(pixels);
    network_free(net);
    matrix_free(res);
    free(values);

    SDL_QuitSubSystem(SDL_INIT_VIDEO);
    SDL_Quit();

    return 0;
}

static void write_and_solve(int *sudoku, char *grid_path, int DIM)
{

    size_t filelen = 1024;
    char *gridfile = (char *)malloc(filelen * sizeof(char));
    strncpy(gridfile, grid_path, filelen - 1);
    if (gridfile[filelen - 1] != '\0')
        errx(1,
            "write_and_solve: overflow happened when creating grid file name");

    strncat(gridfile, "grid", filelen - 1);
    FILE *pfile = fopen(gridfile, "w");
    if (pfile == NULL)
        errx(1, "write_and_solve: failed opening file %s", gridfile);

    char ligne[2 * DIM];

    size_t mod = sqrt(DIM);

    for (int i = 0; i < DIM; i++)
    {
        if (i > 0 && i % mod == 0)
            fputs("\n", pfile);

        size_t pl = 0;
        for (int j = 0; j < DIM; j++)
        {
            if (j > 0 && j % mod == 0)
            {
                ligne[pl] = ' ';
                pl++;
            }
            if (sudoku[i * DIM + j] <= 0)
            {
                ligne[pl] = '.';
                pl++;
            }

            else if (sudoku[i * DIM + j] < 10)
            {
                ligne[pl] = sudoku[i * DIM + j] + 48;
                pl++;
            }
            else
            {
                ligne[pl] = sudoku[i * DIM + j] + 55;
                pl++;
            }
        }
        ligne[pl] = '\0';
        fputs(ligne, pfile);
        fputs("\n", pfile);
    }

    fclose(pfile);
    solve(gridfile, DIM);
    printf("Sucessfully solved the grid!\n");
    free(gridfile);
}

int *convert_multi(
    char *img_path, char *net_path, char *grid_path, double tres)
{

    if (SDL_Init(SDL_INIT_VIDEO) != 0)
        return NULL;

    struct dirent *dir;
    DIR *d = opendir(img_path);
    int *sudoku = NULL;
    if (d)
    {
        network_t *net = network_load(net_path);
        size_t grid_size = (net->inputs - 1) * (net->inputs - 1);
        sudoku = calloc(grid_size, sizeof(int));
        while ((dir = readdir(d)) != NULL)
        {
            if (dir->d_type == 8)
            {
                SDL_Surface *surface = NULL;
                size_t pathlen = 1024;
                char *path = (char *)malloc(pathlen * sizeof(char));
                strncpy(path, img_path, pathlen - 1);
                strncat(path, dir->d_name, pathlen - 1);
                surface = load_image(path);
                double *values = surface_to_grayscale_img(surface);
                matrix_t *pixels = mat_create_fill(net->inputs, 1, values);
                matrix_t *res = compute_results(pixels, net);
                int out = network_get_output(res, tres);

                double sum = 0;
                for (size_t k = 0; k < net->inputs; k++)
                    sum += values[k];

                if (sum == 0)
                    out = 0;

                int index = get_index(dir->d_name);
                sudoku[index] = out;
                matrix_free(res);
                free(values);
                matrix_free(pixels);
                SDL_FreeSurface(surface);
                free(path);
            }
        }
        network_free(net);
        write_and_solve(sudoku, grid_path, 9);
        free(dir);
    }
    else
    {
        printf("Unable to open path: %s\n", img_path);
    }
    closedir(d);
    SDL_QuitSubSystem(SDL_INIT_VIDEO);
    SDL_Quit();
    return sudoku;
}
