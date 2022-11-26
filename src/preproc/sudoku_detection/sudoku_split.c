#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <err.h>
#include <math.h>
#include "sudoku_split.h"
#include "../../utils/matrices/matrix.h"
#include "../color_removal/color_removal.h"
#define UNUSED(x) (void)(x)

// Loads an image in a surface.
// The format of the surface is SDL_PIXELFORMAT_RGB888.
//
// path: Path of the image.
static SDL_Surface *load_image(const char *path)
{
    SDL_Surface *tmp = IMG_Load(path);
    if (tmp == NULL)
        return NULL; // errx(EXIT_FAILURE, "%s", SDL_GetError());

    SDL_Surface *surface
        = SDL_ConvertSurfaceFormat(tmp, SDL_PIXELFORMAT_RGB888, 0);
    SDL_FreeSurface(tmp);

    if (surface == NULL)
        return NULL; // errx(EXIT_FAILURE, "%s", SDL_GetError());
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

static int crop(SDL_Surface *b_w, SDL_Surface **output_p)
{
    SDL_Surface *output = *output_p;

    size_t w = (size_t)output->w;
    size_t h = (size_t)output->h;
    size_t minx = w;
    size_t miny = h;
    size_t maxx = 0;
    size_t maxy = 0;

    SDL_PixelFormat *format = output->format;

    Uint32 *pixels_b_w = b_w->pixels;

    for (size_t i = 0; i < w; i++)
        for (size_t j = 0; j < h; j++)
        {
            Uint8 r, g, b;
            SDL_GetRGB(pixels_b_w[j * w + i], format, &r, &g, &b);
            if ((int)((r + g + b) / 3) == 255)
            {
                if (i < minx)
                    minx = i;
                if (i > maxx)
                    maxx = i;
                if (j < miny)
                    miny = j;
                if (j > maxy)
                    maxy = j;
            }
        }
    if (minx != w && miny != h && maxx != 0 && maxy != 0)
    {
        size_t new_w = maxx - minx > 28 ? maxx - minx : 28;
        size_t new_h = maxy - miny > 28 ? maxy - miny : 28;

        SDL_Surface *new
            = SDL_CreateRGBSurface(0, new_w, new_h, 32, 0, 0, 0, 0);

        SDL_Rect rect;
        rect.x = minx;
        rect.y = miny;
        rect.w = new_w;
        rect.h = new_h;

        SDL_UnlockSurface(output);

        if (SDL_BlitSurface(output, &rect, new, NULL) != 0)
            return 1; // errx(EXIT_FAILURE, "%s", SDL_GetError());

        SDL_LockSurface(output);
        SDL_FreeSurface(*output_p);
        *output_p = new;
    }
    return 0;
}

static int check_square(
    SDL_Surface *surface, size_t x1, size_t x2, size_t y1, size_t y2)
{
    size_t w = surface->w;

    int res = 0;

    SDL_PixelFormat *format = surface->format;
    Uint32 *pixels = surface->pixels;

    for (size_t i = x1; i < x2; i++)
    {
        Uint8 r, g, b;
        SDL_GetRGB(pixels[y1 * w + i], format, &r, &g, &b);
        if (r + g + b == 765)
            res++;
        SDL_GetRGB(pixels[y2 * w + i], format, &r, &g, &b);
        if (r + g + b == 765)
            res++;
    }
    for (size_t j = y1; j < y2; j++)
    {
        Uint8 r, g, b;
        SDL_GetRGB(pixels[j * w + x1], format, &r, &g, &b);
        if (r + g + b == 765)
            res++;
        SDL_GetRGB(pixels[j * w + x2], format, &r, &g, &b);
        if (r + g + b == 765)
            res++;
    }
    return res;
}

static void remove_border(
    SDL_Surface *surface, size_t x1, size_t x2, size_t y1, size_t y2)
{
    size_t w = surface->w;
    size_t h = surface->h;

    SDL_PixelFormat *format = surface->format;
    Uint32 *pixels = surface->pixels;

    for (size_t j = 0; j < y1; j++)
        for (size_t i = 0; i < w; i++)
            pixels[j * w + i] = SDL_MapRGB(format, 0, 0, 0);

    for (size_t j = y2; j < h; j++)
        for (size_t i = 0; i < w; i++)
            pixels[j * w + i] = SDL_MapRGB(format, 0, 0, 0);

    for (size_t j = 0; j < h; j++)
        for (size_t i = 0; i < x1; i++)
            pixels[j * w + i] = SDL_MapRGB(format, 0, 0, 0);

    for (size_t j = 0; j < h; j++)
        for (size_t i = x2; i < w; i++)
            pixels[j * w + i] = SDL_MapRGB(format, 0, 0, 0);
}

static void clean_border(SDL_Surface *surface, SDL_Surface *gray)
{
    size_t w = surface->w;
    size_t h = surface->h;

    size_t x1 = w / 2 - 1, x2 = w / 2 + 1, y1 = h / 2 - 1, y2 = h / 2 + 1;

    while (check_square(surface, x1, x2, y1, y2) == 0
           && !(x1 == 0 && x2 == w - 1 && y1 == 0 && y2 == h - 1))
    {
        if (x1 > 0)
            x1 -= 1;
        if (x2 < w - 1)
            x2 += 1;
        if (y1 > 0)
            y1 -= 1;
        if (y2 < h - 1)
            y2 += 1;
    }

    if ((x2 - x1) * (y2 - y1) < (w * h) / 2)
    {
        while (check_square(surface, x1, x2, y1, y2) != 0
               && !(x1 == 0 && x2 == w - 1 && y1 == 0 && y2 == h - 1))
        {
            if (x1 > 0)
                x1 -= 1;
            if (x2 < w - 1)
                x2 += 1;
            if (y1 > 0)
                y1 -= 1;
            if (y2 < h - 1)
                y2 += 1;
        }

        remove_border(surface, x1, x2, y1, y2);
        remove_border(gray, x1, x2, y1, y2);
    }
    else
    {
        remove_border(surface, w / 2, w / 2 + 1, h / 2, h / 2 + 1);
        remove_border(gray, w / 2, w / 2 + 1, h / 2, h / 2 + 1);
    }
}

static void average(size_t i, size_t j, double ratio_h, double ratio_w,
    size_t w, SDL_Surface *surface, int *avr, int *avg, int *avb)
{
    SDL_PixelFormat *format = surface->format;
    Uint32 *pixels = surface->pixels;

    *avr = 0;
    *avg = 0;
    *avb = 0;
    size_t count = 0;
    for (size_t x = 0; x < (size_t)ratio_w; x++)
    {
        if ((i + x) * ratio_w >= surface->w)
            continue;
        for (size_t y = 0; y < (size_t)ratio_h; y++)
        {
            if ((j + y) * ratio_h >= surface->h)
                continue;
            Uint8 r, g, b;
            SDL_GetRGB(pixels[(size_t)((j + y) * ratio_h) * w
                              + (size_t)((i + x) * ratio_w)],
                format, &r, &g, &b);
            *avr += r;
            *avg += g;
            *avb += b;
            count++;
        }
    }
    *avr = floor(*avr / count);
    *avg = floor(*avg / count);
    *avb = floor(*avb / count);
}

void scale_down(
    SDL_Surface **surface_p, size_t w, size_t h, size_t new_w, size_t new_h)
{
    SDL_Surface *surface = *surface_p;
    SDL_Surface *output
        = SDL_CreateRGBSurface(0, (int)new_w, (int)new_h, 32, 0, 0, 0, 0);

    SDL_PixelFormat *format_output = output->format;

    Uint32 *pixels_output = output->pixels;

    double ratio_h = ((double)h / new_h);
    double ratio_w = ((double)w / new_w);

    for (size_t j = 0; j < new_h; j++)
        for (size_t i = 0; i < new_w; i++)
        {
            int r, g, b;
            average(i, j, ratio_h, ratio_w, w, surface, &r, &g, &b);
            pixels_output[j * new_w + i] = SDL_MapRGB(format_output, r, g, b);
        }

    SDL_FreeSurface(*surface_p);
    *surface_p = output;
}

static void add_border(
    SDL_Surface **surface, size_t w, size_t h, size_t border)
{
    SDL_Surface *new = SDL_CreateRGBSurface(
        0, w + (2 * border), h + (2 * border), 32, 0, 0, 0, 0);

    SDL_PixelFormat *format = new->format;
    Uint32 *pixels = new->pixels;

    SDL_PixelFormat *old_format = (*surface)->format;
    Uint32 *old_pixels = (*surface)->pixels;

    size_t nw = new->w, nh = new->h;

    for (size_t i = 0; i < border; i++)
    {
        size_t start_offset = i * nw, end_offset = (nh - i - 1) * nw;
        for (size_t j = 0; j < nw; j++)
        {
            pixels[start_offset + j] = SDL_MapRGB(format, 0, 0, 0);
            pixels[end_offset + j] = SDL_MapRGB(format, 0, 0, 0);
        }
    }
    for (size_t i = 0; i < border; i++)
    {
        size_t end_offset = nw - i - 1;
        for (size_t j = 0; j < nh; j++)
        {
            pixels[j * nw + i] = SDL_MapRGB(format, 0, 0, 0);
            pixels[j * nw + end_offset] = SDL_MapRGB(format, 0, 0, 0);
        }
    }

    for (size_t i = 0; i < w; i++)
        for (size_t j = 0; j < h; j++)
        {
            Uint8 r, g, b;
            SDL_GetRGB(old_pixels[j * w + i], old_format, &r, &g, &b);
            pixels[(j + border) * nw + (i + border)]
                = SDL_MapRGB(format, r, g, b);
        }

    SDL_FreeSurface(*surface);
    *surface = new;
}

static int square(SDL_Surface *b_w, SDL_Surface *gray, size_t x1, size_t y1,
    size_t x2, size_t y2, size_t name, char path[30])
{
    SDL_Surface *output
        = SDL_CreateRGBSurface(0, x2 - x1, y2 - y1, 32, 0, 0, 0, 0);
    SDL_Surface *output_b_w
        = SDL_CreateRGBSurface(0, x2 - x1, y2 - y1, 32, 0, 0, 0, 0);

    SDL_PixelFormat *format = gray->format;
    SDL_PixelFormat *format_output = output->format;
    SDL_PixelFormat *format_b_w = b_w->format;
    SDL_PixelFormat *format_output_b_w = output_b_w->format;

    size_t w = gray->w;

    Uint32 *pixels = gray->pixels;
    Uint32 *pixels_output = output->pixels;
    Uint32 *pixels_b_w = b_w->pixels;
    Uint32 *pixels_output_b_w = output_b_w->pixels;

    SDL_LockSurface(gray);
    SDL_LockSurface(output);
    SDL_LockSurface(b_w);
    SDL_LockSurface(output_b_w);

    for (size_t i = 0; i < x2 - x1; i++)
    {
        for (size_t j = 0; j < y2 - y1; j++)
        {
            Uint8 r, g, b;
            SDL_GetRGB(pixels[(y1 + j) * w + x1 + i], format, &r, &g, &b);

            pixels_output[j * (x2 - x1) + i]
                = SDL_MapRGB(format_output, r, g, b);

            SDL_GetRGB(
                pixels_b_w[(y1 + j) * w + x1 + i], format_b_w, &r, &g, &b);

            pixels_output_b_w[j * (x2 - x1) + i]
                = SDL_MapRGB(format_output_b_w, r, g, b);
        }
    }
    clean_border(output_b_w, output);
    if (crop(output_b_w, &output) != 0)
        return 1;
    if (output == NULL)
        return 1;
    if (output->w < 28 || output->h < 28)
        add_border(&output, output->w, output->h,
            (size_t)fmax((double)(28 - output->w), (double)(28 - output->h)));
    scale_down(&output, output->w, output->h, 20, 20);
    add_border(&output, 20, 20, 4);

    SDL_UnlockSurface(gray);
    SDL_UnlockSurface(output);
    SDL_UnlockSurface(b_w);
    SDL_UnlockSurface(output_b_w);
    SDL_FreeSurface(output_b_w);

    char out[50];
    sprintf(out, "%s%zu.png", path, name);

    IMG_SavePNG(output, out);
    SDL_FreeSurface(output);
    return 0;
}

static void cut_squares(matrix_t *inter, SDL_Surface *b_w, SDL_Surface *gray,
    size_t len, char path[30])
{
    size_t name = 0;
    size_t stop = len * len - len - 1;
    for (size_t i = 0; i < len * len; i++)
    {
        if (i >= stop)
            break;
        if (i % len != len - 1)
        {
            square(b_w, gray, mat_el_at(inter, i, 0), mat_el_at(inter, i, 1),
                mat_el_at(inter, i + len + 1, 0),
                mat_el_at(inter, i + len + 1, 1), name, path);
            name++;
        }
    }
}

static void insert_line(
    size_t lines[10][2], size_t i, size_t rho, size_t theta)
{
    for (size_t j = 9; j > i; j--)
    {
        size_t temp[2] = {lines[j - 1][0], lines[j - 1][1]};
        lines[j - 1][0] = lines[j][0];
        lines[j - 1][1] = lines[j][1];
        lines[j][0] = temp[0];
        lines[j][1] = temp[1];
    }
    lines[i][0] = rho;
    lines[i][1] = theta;
}

// lines should have 4 lines stored already
static void add_missing_lines(size_t lines[10][2])
{
    for (size_t i = 0; i < 8; i += 3)
    {
        size_t third = (size_t)((lines[i + 1][0] - lines[i][0]) / 3);
        insert_line(lines, i + 1, lines[i][0] + third, lines[i][1]);
        insert_line(lines, i + 2, lines[i][0] + 2 * third, lines[i][1]);
    }
}

// stores lines' coordinates in the "lines" matrix
static int store_lines(matrix_t *acc, size_t rhos, ssize_t lines[20][2],
    double maximum, int thres)
{
    for (size_t i = 0; i < 20; i++)
    {
        lines[i][0] = -1;
        lines[i][1] = -1;
    }

    double ratio = maximum / 1000;
    size_t x = 0;
    for (size_t j = 0; j < 360; j++)
        for (size_t i = 0; i < rhos; i++)
        {
            double val = mat_el_at(acc, i, j) / ratio;
            if (val > thres)
            {
                if (x >= 20)
                    return -1; // errx(1, "wrong number of lines");
                lines[x][0] = i;
                lines[x][1] = j;
                x++;
            }
        }
    if (x % 2 != 0)
        return -1; // errx(1, "odd number of lines");
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
static int separate_lines(
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
                            return 1; // errx(1, "solo line");
                        }
                    }
                }
            }
        }
    }
    sort_list(lines1, len / 2);
    sort_list(lines2, len / 2);
    return 0;
}

static int intersections(matrix_t *acc, size_t rhos, SDL_Surface *surface,
    SDL_Surface *sudoku, double maximum, int thres, char path[30],
    int get_rotation)
{
    ssize_t lines[20][2];
    int l = store_lines(acc, rhos, lines, maximum, thres);
    if (l < 0)
        return 1;
    size_t len = (size_t)l;

    size_t lines1[10][2];
    size_t lines2[10][2];
    if (separate_lines(lines1, lines2, len, lines) != 0)
        return 1;

    if (len == 8)
    {
        add_missing_lines(lines1);
        add_missing_lines(lines2);
        len = 20;
    }

    matrix_t *inter = matrix_create((len / 2) * (len / 2), 2, 0);

    mat_transform(inter, (mat_transform_t)set_minus_one, 1);

    draw_line(surface, inter, lines1, len / 2);
    size_t n = draw_line(surface, inter, lines2, len / 2);

    if (n < (len / 2) * (len / 2))
        return 1; // errx(1, "404: Intersection not found");

    if (!get_rotation)
        cut_squares(inter, surface, sudoku, len / 2, path);

    matrix_free(inter);
    return 0;
}

static void fuze_lines(matrix_t *acc, size_t rhos, double maximum, int thres)
{
    double ratio = maximum / 1000;
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
                mat_set_el(acc, i, j, maximum);
            }
        }
}

static int count_lines(matrix_t *acc, size_t rhos, double maximum, int thres)
{
    int count = 0;
    double ratio = maximum / 1000;
    for (size_t i = 0; i < rhos; i++)
        for (size_t j = 0; j < 360; j++)
        {
            double val = mat_el_at(acc, i, j) / ratio;
            if (val > thres) // considered a real line
                count++;
        }
    return count;
}

static double get_max(matrix_t *acc, size_t rhos, int *thres)
{
    double maximum = 0;
    for (size_t i = 0; i < rhos; i++)
        for (size_t j = 0; j < 360; j++)
        {
            double el = mat_el_at(acc, i, j);
            if (el > maximum)
                maximum = el;
        }
    fuze_lines(acc, rhos, maximum, *thres);
    if (count_lines(acc, rhos, maximum, *thres) < 8)
    {
        *thres -= 50;
        return get_max(acc, rhos, thres);
    }
    return maximum;
}

static int rotation(matrix_t *acc, size_t rho, double maximum)
{
    for (size_t t = 0; t < 360; t++)
        for (size_t r = 0; r < rho; r++)
            if (mat_el_at(acc, r, t) == maximum)
                return t;
    return -1;
}
static int line_detection(
    SDL_Surface *surface, SDL_Surface *sudoku, int get_rotation, char path[30])
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

    int thres = 600;

    double maximum = get_max(acc, diag, &thres);

    int value = intersections(
        acc, diag, surface, sudoku, maximum, thres, path, get_rotation);

    int theta = 0;

    if (get_rotation)
        theta = rotation(acc, diag, maximum);

    matrix_free(acc);
    if (value == 1)
        return -2;
    return theta;
}

int sudoku_split(char *black_white, char *grayscale, char *path)
{
    // Initializes the SDL.
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
        return 1; // errx(EXIT_FAILURE, "%s", SDL_GetError());

    SDL_Surface *b_w = load_image(black_white);
    if (b_w == NULL)
        return 1;
    SDL_Surface *gray = load_image(grayscale);
    if (gray == NULL)
        return 1;

    char save_path[30];
    strcpy(save_path, path);

    int value = line_detection(b_w, gray, 0, save_path);

    SDL_FreeSurface(b_w);
    SDL_FreeSurface(gray);

    SDL_Quit();
    if (value == -2)
        return 1;
    return 0;
}

static int modulo(int x, int y)
{
    int res = x % y;
    if (res > y / 2)
        res -= y;
    return res;
}

int get_rotation(SDL_Surface *surface)
{
    char save_path[30] = {0};
    int theta = line_detection(surface, surface, 1, save_path);
    if (theta < 0)
        return -1;
    return modulo(theta, 90);
}
