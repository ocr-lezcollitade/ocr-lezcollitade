#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <err.h>
#include <math.h>
#include "sudoku_split.h"
#include "../../utils/matrices/matrix.h"
#define UNUSED(x) (void)(x)

int thres = 600;
char save_path[30];

// Loads an image in a surface.
// The format of the surface is SDL_PIXELFORMAT_RGB888.
//
// path: Path of the image.
static SDL_Surface *load_image(const char *path)
{
    SDL_Surface *tmp = IMG_Load(path);
    if (tmp == NULL)
        errx(EXIT_FAILURE, "%s", SDL_GetError());

    SDL_Surface *surface
        = SDL_ConvertSurfaceFormat(tmp, SDL_PIXELFORMAT_RGB888, 0);
    SDL_FreeSurface(tmp);

    if (surface == NULL)
        errx(EXIT_FAILURE, "%s", SDL_GetError());
    return surface;
}

static void swap(size_t *xp, size_t *yp)
{
    int temp = *xp;
    *xp = *yp;
    *yp = temp;
}

// Function to perform Selection Sort
static void sort_list(size_t arr[][2], int n)
{
    int i, j, min_idx;

    // One by one move boundary of unsorted subarray
    for (i = 0; i < n - 1; i++)
    {

        // Find the minimum element in unsorted array
        min_idx = i;
        for (j = i + 1; j < n; j++)
            if (arr[j][0] < arr[min_idx][0])
                min_idx = j;

        // Swap the found minimum element
        // with the first element
        swap(&arr[min_idx][0], &arr[i][0]);
        swap(&arr[min_idx][1], &arr[i][1]);
    }
}

static size_t draw_line_horizontal(SDL_Surface *surface, matrix_t *inter,
    size_t rho, size_t theta, size_t h, size_t w, size_t n)
{
    SDL_PixelFormat *format = surface->format;
    Uint32 *pixels = surface->pixels;
    for (size_t y = 0; y < h; y++)
    {
        double arg = (theta * M_PI) / 180;
        size_t dx = (size_t)floor((rho - y * sin(arg)) / cos(arg) + 0.5);
        if (dx > 0 && dx < w)
        {
            Uint8 r, g, b;
            SDL_GetRGB(pixels[y * w + dx], format, &r, &g, &b);
            if (r + g + b == 255)
            {
                mat_set_el(inter, n, 0, dx);
                mat_set_el(inter, n, 1, y);
                n++;
            }
            else
                pixels[y * w + dx] = SDL_MapRGB(format, 255, 0, 0);
        }
    }
    return n;
}

static size_t draw_line_vertical(SDL_Surface *surface, matrix_t *inter,
    size_t rho, size_t theta, size_t h, size_t w, size_t n)
{
    SDL_PixelFormat *format = surface->format;
    Uint32 *pixels = surface->pixels;
    for (size_t x = 0; x < w; x++)
    {
        double arg = (theta * M_PI) / 180;
        size_t dy = (size_t)floor((rho - x * cos(arg)) / sin(arg) + 0.5);
        if (dy > 0 && dy < h)
        {
            Uint8 r, g, b;
            SDL_GetRGB(pixels[dy * w + x], format, &r, &g, &b);
            if (r + g + b == 255)
            {
                mat_set_el(inter, n, 0, x);
                mat_set_el(inter, n, 1, dy);
                n++;
            }
            else
                pixels[dy * w + x] = SDL_MapRGB(format, 255, 0, 0);
        }
    }
    return n;
}

static size_t draw_line(
    SDL_Surface *surface, matrix_t *inter, size_t lines[][2], size_t len)
{
    size_t w = (size_t)surface->w;
    size_t h = (size_t)surface->h;
    size_t n = 0;

    SDL_LockSurface(surface);
    for (size_t i = 0; i < len; i++)
    {
        size_t rho = lines[i][0];
        size_t theta = lines[i][1];
        if (theta < 45 || (theta > 135 && theta < 225) || theta > 315)
            n = draw_line_horizontal(surface, inter, rho, theta, h, w, n);
        else
            n = draw_line_vertical(surface, inter, rho, theta, h, w, n);
    }
    SDL_UnlockSurface(surface);
    return n;
}

static double set_minus_one(double input)
{
    UNUSED(input);
    return -1;
}

static void remove_border(SDL_Surface *surface, size_t w, size_t h)
{
    size_t pixel_del_w = (size_t)round(0.2 * w);
    size_t pixel_del_h = (size_t)round(0.1 * h);

    SDL_PixelFormat *format = surface->format;

    Uint32 *pixels = surface->pixels;

    for (size_t j = 0; j < pixel_del_h; j++)
        for (size_t i = 0; i < w; i++)
            pixels[j * w + i] = SDL_MapRGB(format, 0, 0, 0);

    for (size_t j = h - pixel_del_h; j < h; j++)
        for (size_t i = 0; i < w; i++)
            pixels[j * w + i] = SDL_MapRGB(format, 0, 0, 0);

    for (size_t j = 0; j < h; j++)
        for (size_t i = 0; i < pixel_del_w; i++)
            pixels[j * w + i] = SDL_MapRGB(format, 0, 0, 0);

    for (size_t j = 0; j < h; j++)
        for (size_t i = w - pixel_del_w; i < w; i++)
            pixels[j * w + i] = SDL_MapRGB(format, 0, 0, 0);
}

static void average(size_t i, size_t j, double ratio_h, double ratio_w,
    size_t w, SDL_PixelFormat *format, Uint32 *pixels, int *avr, int *avg,
    int *avb)
{
    *avr = 0;
    *avg = 0;
    *avb = 0;
    size_t count = 0;
    for (size_t x = 0; x < (size_t)ratio_w; x++)
        for (size_t y = 0; y < (size_t)ratio_h; y++)
        {
            Uint8 r, g, b;
            SDL_GetRGB(pixels[(size_t)((j + y) * ratio_h) * w
                              + (size_t)((i + x) * ratio_w)],
                format, &r, &g, &b);
            *avr += r;
            *avg += g;
            *avb += b;
            count++;
        }
    *avr = floor(*avr / count);
    *avg = floor(*avg / count);
    *avb = floor(*avb / count);
}

SDL_Surface *scale_down(
    SDL_Surface *surface, size_t w, size_t h, size_t new_w, size_t new_h)
{
    SDL_Surface *output
        = SDL_CreateRGBSurface(0, (int)new_w, (int)new_h, 32, 0, 0, 0, 0);

    SDL_PixelFormat *format = surface->format;
    SDL_PixelFormat *format_output = output->format;

    Uint32 *pixels = surface->pixels;
    Uint32 *pixels_output = output->pixels;

    double ratio_h = ((double)h / new_h);
    double ratio_w = ((double)w / new_w);

    for (size_t j = 0; j < new_h; j++)
        for (size_t i = 0; i < new_w; i++)
        {
            int r, g, b;
            average(i, j, ratio_h, ratio_w, w, format, pixels, &r, &g, &b);
            pixels_output[j * new_w + i] = SDL_MapRGB(format_output, r, g, b);
        }

    return output;
}

static void square(SDL_Surface *surface, size_t x1, size_t y1, size_t x2,
    size_t y2, size_t name)
{
    SDL_Surface *output
        = SDL_CreateRGBSurface(0, x2 - x1, y2 - y1, 32, 0, 0, 0, 0);

    SDL_PixelFormat *format = surface->format;
    SDL_PixelFormat *format_output = output->format;

    size_t w = surface->w;

    Uint32 *pixels = surface->pixels;
    Uint32 *pixels_output = output->pixels;

    SDL_LockSurface(surface);
    SDL_LockSurface(output);

    for (size_t i = 0; i < x2 - x1; i++)
    {
        for (size_t j = 0; j < y2 - y1; j++)
        {
            Uint8 r, g, b;
            SDL_GetRGB(pixels[(y1 + j) * w + x1 + i], format, &r, &g, &b);

            pixels_output[j * (x2 - x1) + i]
                = SDL_MapRGB(format_output, r, g, b);
        }
    }

    remove_border(output, x2 - x1, y2 - y1);
    SDL_Surface *final = scale_down(output, x2 - x1, y2 - y1, 28, 28);

    SDL_UnlockSurface(surface);
    SDL_UnlockSurface(output);
    SDL_FreeSurface(output);

    char out[50];
    sprintf(out, "%s%zu.png", save_path, name);

    IMG_SavePNG(final, out);
    SDL_FreeSurface(final);
}

static void cut_squares(matrix_t *inter, SDL_Surface *surface, size_t len)
{
    size_t name = 0;
    size_t stop = len * len - len - 1;
    for (size_t i = 0; i < len * len; i++)
    {
        if (i >= stop)
            break;
        if (i % len != len - 1)
        {
            square(surface, mat_el_at(inter, i, 0), mat_el_at(inter, i, 1),
                mat_el_at(inter, i + len + 1, 0),
                mat_el_at(inter, i + len + 1, 1), name);
            name++;
        }
    }
}

static size_t store_lines(
    matrix_t *acc, double max, size_t rhos, ssize_t lines[20][2])
{
    for (size_t i = 0; i < 20; i++)
    {
        lines[i][0] = -1;
        lines[i][1] = -1;
    }

    double ratio = max / 1000;
    size_t x = 0;
    for (size_t j = 0; j < 360;
         j++) // stores lines' coordinates in the "lines" matrix
        for (size_t i = 0; i < rhos; i++)
        {
            double val = mat_el_at(acc, i, j) / ratio;
            if (val > thres)
            {
                if (x >= 20)
                    errx(1, "wrong number of lines");
                lines[x][0] = i;
                lines[x][1] = j;
                x++;
            }
        }
    if (x % 2 != 0)
        errx(1, "odd number of lines");
    return x;
}

static size_t set_line_coordinates(
    size_t array[][2], size_t i1, size_t i, ssize_t lines[20][2])
{
    array[i1][0] = lines[i][0];
    array[i1][1] = lines[i][1];
    return i1 + 1;
}

// separates vertical and horizontal lines + orders them in increasing order
// according to their distance from origin t1, t2 = approximated theta for one
// of the lines' type
static void separate_lines(
    size_t lines1[][2], size_t lines2[][2], size_t len, ssize_t lines[20][2])
{
    int t1 = -1, t2 = -1;
    size_t i1 = 0, i2 = 0;

    for (size_t i = 0; i < len; i++)
    {
        if (t1 == -1)
        {
            t1 = lines[i][1];
            i1 = set_line_coordinates(lines1, i1, i, lines);
        }
        else
        {
            int lt = (int)lines[i][1] % 180;
            if ((lt > t1 - 10 && lt < t1 + 10)
                || (lt > t1 - 10 + 180 && lt < t1 + 10 + 180)
                || (lt > t1 - 10 - 180 && lt < t1 + 10 - 180))
            {
                i1 = set_line_coordinates(lines1, i1, i, lines);
            }
            else
            {
                if (t2 == -1)
                {
                    t2 = lines[i][1];
                    i2 = set_line_coordinates(lines2, i2, i, lines);
                }
                else
                {
                    if ((lt > t1 - 10 && lt < t1 + 10)
                        || (lt > t1 - 10 + 180 && lt < t1 + 10 + 180)
                        || (lt > t1 - 10 - 180 && lt < t1 + 10 - 180))
                    {
                        i1 = set_line_coordinates(lines1, i1, i, lines);
                    }
                    else
                    {
                        if ((lt > t2 - 10 && lt < t2 + 10)
                            || (lt > t2 - 10 + 180 && lt < t2 + 10 + 180)
                            || (lt > t2 - 10 - 180 && lt < t2 + 10 - 180))
                        {
                            i2 = set_line_coordinates(lines2, i2, i, lines);
                        }
                        else
                        {
                            errx(1, "solo line");
                        }
                    }
                }
            }
        }
    }
    sort_list(lines1, len / 2);
    sort_list(lines2, len / 2);
}

static void intersections(matrix_t *acc, double max, size_t rhos,
    SDL_Surface *surface, SDL_Surface *sudoku)
{
    ssize_t lines[20][2];
    size_t len = store_lines(acc, max, rhos, lines);

    size_t lines1[len / 2][2];
    size_t lines2[len / 2][2];
    separate_lines(lines1, lines2, len, lines);

    matrix_t *inter = matrix_create((len / 2) * (len / 2), 2, 0);

    mat_transform(inter, (mat_transform_t)set_minus_one, 1);

    draw_line(surface, inter, lines1, len / 2);
    size_t n = draw_line(surface, inter, lines2, len / 2);

    if (n < (len / 2) * (len / 2))
        errx(1, "404: Intersection not found");

    cut_squares(inter, sudoku, len / 2);

    matrix_free(inter);
}

static void fuze_lines(matrix_t *acc, double max, size_t rhos)
{
    double ratio = max / 1000;
    size_t del_t = 70;
    size_t del_r = 30 * rhos / 1000;
    for (size_t j = 0; j < 360; j++)
        for (size_t i = 0; i < rhos; i++)
        {
            double val = mat_el_at(acc, i, j) / ratio;
            if (val > thres) // considered a real line
            {
                for (int x = (int)(i - del_r); x < (int)(i + del_r); x++)
                {
                    for (int y = (int)(j - del_t); y < (int)(j + del_t); y++)
                    {
                        if (x < (int)rhos
                            && ((x >= 0 && y >= 0) || (x >= 0 && y > 0)))
                        {
                            mat_set_el(acc, x, y % 360, 0);
                            mat_set_el(acc, x, (y + 180) % 360, 0);
                        }
                    }
                }
                mat_set_el(acc, i, j, max);
            }
        }
}

static int count_lines(matrix_t *acc, double max, size_t rhos)
{
    int count = 0;
    double ratio = max / 1000;
    for (size_t i = 0; i < rhos; i++)
        for (size_t j = 0; j < 360; j++)
        {
            double val = mat_el_at(acc, i, j) / ratio;
            if (val > thres) // considered a real line
                count++;
        }
    return count;
}

static double get_max(matrix_t *acc, size_t rhos)
{
    double max = 0;
    for (size_t i = 0; i < rhos; i++)
        for (size_t j = 0; j < 360; j++)
        {
            double el = mat_el_at(acc, i, j);
            if (el > max)
                max = el;
        }
    fuze_lines(acc, max, rhos);
    if (count_lines(acc, max, rhos) < 20)
    {
        thres -= 50;
        return get_max(acc, rhos);
    }
    return max;
}

static void line_detection(SDL_Surface *surface, SDL_Surface *sudoku)
{
    SDL_PixelFormat *format = surface->format;
    Uint32 *pixels = surface->pixels;
    size_t w = (size_t)surface->w;
    size_t h = (size_t)surface->h;
    size_t diag = sqrt(w * w + h * h);
    matrix_t *acc = matrix_create(diag, 360, 1);

    SDL_LockSurface(surface);
    for (size_t y = 0; y < h; y++)
        for (size_t x = 0; x < w; x++)
        {
            Uint8 r, g, b;
            SDL_GetRGB(pixels[y * w + x], format, &r, &g, &b);

            if (r + g + b == 765) // pixel is white
                for (int t = 0; t < 360; t++)
                {
                    double arg = (t * M_PI) / 180;
                    size_t r = (size_t)(x * cos(arg) + y * sin(arg));
                    if (r < diag)
                        mat_set_el(acc, r, t, mat_el_at(acc, r, t) + 1);
                }
        }
    SDL_UnlockSurface(surface);

    double max = get_max(acc, diag);

    intersections(acc, max, diag, surface, sudoku);

    matrix_free(acc);
}

void sudoku_split(char *black_white, char *grayscale, char *path)
{
    // Initializes the SDL.
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
        errx(EXIT_FAILURE, "%s", SDL_GetError());

    SDL_Surface *surface = load_image(black_white);
    SDL_Surface *sudoku = load_image(grayscale);

    strcpy(save_path, path);

    line_detection(surface, sudoku);

    SDL_FreeSurface(surface);
    SDL_FreeSurface(sudoku);

    SDL_Quit();
}
