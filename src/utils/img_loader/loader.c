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

double *surface_to_grayscale_img(size_t nb, SDL_Surface *surface)
{
    double *values = malloc(sizeof(double) * nb);
    Uint32 *pixels = surface->pixels;
    SDL_PixelFormat *format = surface->format;

    for (size_t i = 0; i < nb; i++)
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

static int get_index(char *name, int DIM)
{
    char *temp = malloc(sizeof(char));
    int i = 0;
    while (name[i] != '.')
    {
        temp[i] = name[i];
        i++;
        temp = realloc(temp, i + 1);
    }
    temp[i] = 0;
    int val = atoi(temp);
    free(temp);
    if (val > DIM * DIM - 1)
        return -1;
    return val;
}

int test_single(network_t *net, char *img_path, double tres)
{

    if (SDL_Init(SDL_INIT_VIDEO) != 0)
        errx(EXIT_FAILURE, "%s", SDL_GetError());

    SDL_Surface *surface = load_image(img_path);

    double *values = surface_to_grayscale_img(net->inputs, surface);

    SDL_FreeSurface(surface);

    print(values);

    int out = convert_single(net, values, tres);

    printf("It's a ");
    if (out < 0)
        printf("%d", out);
    else if (out < 10)
        printf("%c", out + '0');
    else
        printf("%c", out - 10 + 'A');
    printf("!\n");

    SDL_QuitSubSystem(SDL_INIT_VIDEO);
    SDL_Quit();

    free(values);

    return 0;
}

int convert_single(network_t *net, double *values, double tres)
{

    matrix_t *pixels = mat_create_fill(net->inputs, 1, values);
    matrix_t *res = compute_results(pixels, net);
    int out = network_get_output(res, tres);

    double sum = 0;
    for (size_t k = 0; k < net->inputs; k++)
        sum += values[k];

    if (sum == 0)
        out = 0;

    matrix_free(pixels);
    matrix_free(res);
    return out;
}

int *convert_multi(char *img_path, network_t *net, double tres)
{

    if (SDL_Init(SDL_INIT_VIDEO) != 0)
        return NULL;

    int DIM = net->layers[net->layer_count - 1]->count - 1;
    struct dirent *dir;
    DIR *d = opendir(img_path);
    int *sudoku = NULL;
    if (d)
    {
        size_t grid_size = DIM * DIM;
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

                double *values
                    = surface_to_grayscale_img(net->inputs, surface);

                int out = convert_single(net, values, tres);

                int index = get_index(dir->d_name, DIM);

                if (index > -1)
                    sudoku[index] = out;

                free(values);
                SDL_FreeSurface(surface);
                free(path);
            }
        }
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
