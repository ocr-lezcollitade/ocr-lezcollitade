#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <err.h>
#include <gtk/gtk.h>
#include <math.h>
#include "sudoku_split.h"
#include "../../ui/ui.h"
#include "../color_removal/color_removal.h"
#define UNUSED(x) (void)(x)
#define ERROR_CODE 4242
#define WHITE 765
#define ALL_LINES(dim) (dim + 1)
#define BIG_LINES(dim) (sqrt(dim) + 1)
static int modulo(int x, int y);

// Loads an image in a surface.
// The format of the surface is SDL_PIXELFORMAT_RGB888.
//
// path: Path of the image.
static SDL_Surface *load_image(const char *path)
{
    SDL_Surface *tmp = IMG_Load(path);
    if (tmp == NULL)
        return NULL;

    SDL_Surface *surface
        = SDL_ConvertSurfaceFormat(tmp, SDL_PIXELFORMAT_RGB888, 0);
    SDL_FreeSurface(tmp);

    if (surface == NULL)
        return NULL;
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

static size_t draw_line_vertical(SDL_Surface *surface, matrix_t *inter,
    size_t rho, int theta, size_t h, size_t w, size_t n)
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
                y++;
            }
            else
                pixels[y * w + dx] = SDL_MapRGB(format, 255, 0, 0);
        }
    }
    return n;
}

static size_t draw_line_horizontal(SDL_Surface *surface, matrix_t *inter,
    size_t rho, int theta, size_t h, size_t w, size_t n)
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
                x++;
            }
            else if (dy != 0)
            {
                SDL_GetRGB(pixels[(dy - 1) * w + x], format, &r, &g, &b);
                if (g + b == 0 && r == 255)
                {
                    mat_set_el(inter, n, 0, x);
                    mat_set_el(inter, n, 1, dy);
                    n++;
                    x++;
                }
                else
                    pixels[dy * w + x] = SDL_MapRGB(format, 255, 0, 0);
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
        int theta = lines[i][1];
        if (rho == 0)
            rho = 1;
        if (theta < 45 || (theta > 135 && theta < 225) || theta > 315)
            n = draw_line_vertical(
                surface, inter, rho, modulo(theta, 180), h, w, n);
        else
            n = draw_line_horizontal(
                surface, inter, rho, theta % 180, h, w, n);
    }
    SDL_UnlockSurface(surface);
    return n;
}

static double set_minus_one(double input)
{
    UNUSED(input);
    return -1;
}

static int crop(SDL_Surface **b_w_p, SDL_Surface **output_p)
{
    SDL_Surface *output = *output_p;
    SDL_Surface *b_w = *b_w_p;

    size_t w = (size_t)output->w;
    size_t h = (size_t)output->h;
    size_t minx = w;
    size_t miny = h;
    size_t maxx = 0;
    size_t maxy = 0;

    SDL_PixelFormat *format_b_w = b_w->format;

    Uint32 *pixels_b_w = b_w->pixels;

    for (size_t i = 0; i < w; i++)
        for (size_t j = 0; j < h; j++)
        {
            Uint8 r, g, b;
            SDL_GetRGB(pixels_b_w[j * w + i], format_b_w, &r, &g, &b);
            if (r + g + b == WHITE)
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
        SDL_Surface *new_b_w
            = SDL_CreateRGBSurface(0, new_w, new_h, 32, 0, 0, 0, 0);

        SDL_Rect rect;
        rect.x = minx;
        rect.y = miny;
        rect.w = new_w;
        rect.h = new_h;

        SDL_UnlockSurface(output);
        SDL_UnlockSurface(b_w);

        if (SDL_BlitSurface(output, &rect, new, NULL) != 0)
            return 1;
        if (SDL_BlitSurface(b_w, &rect, new_b_w, NULL) != 0)
            return 1;

        SDL_LockSurface(output);
        SDL_LockSurface(b_w);
        SDL_FreeSurface(*output_p);
        SDL_FreeSurface(*b_w_p);
        *output_p = new;
        *b_w_p = new_b_w;
    }
    return 0;
}

static int check_line_v(SDL_Surface *surface, size_t x, size_t y1, size_t y2)
{
    size_t w = surface->w;

    int res = 0;

    SDL_PixelFormat *format = surface->format;
    Uint32 *pixels = surface->pixels;

    for (size_t j = y1; j < y2; j++)
    {
        Uint8 r, g, b;
        SDL_GetRGB(pixels[j * w + x], format, &r, &g, &b);
        if (r + g + b == WHITE)
            res++;
    }
    return res;
}

static int check_line_h(SDL_Surface *surface, size_t x1, size_t x2, size_t y)
{
    size_t w = surface->w;

    int res = 0;

    SDL_PixelFormat *format = surface->format;
    Uint32 *pixels = surface->pixels;

    for (size_t i = x1; i < x2; i++)
    {
        Uint8 r, g, b;
        SDL_GetRGB(pixels[y * w + i], format, &r, &g, &b);
        if (r + g + b == WHITE)
            res++;
    }
    return res;
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
        if (r + g + b == WHITE)
            res++;
        SDL_GetRGB(pixels[y2 * w + i], format, &r, &g, &b);
        if (r + g + b == WHITE)
            res++;
    }
    for (size_t j = y1; j < y2; j++)
    {
        Uint8 r, g, b;
        SDL_GetRGB(pixels[j * w + x1], format, &r, &g, &b);
        if (r + g + b == WHITE)
            res++;
        SDL_GetRGB(pixels[j * w + x2], format, &r, &g, &b);
        if (r + g + b == WHITE)
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

static void picture_black(SDL_Surface *surface)
{
    size_t w = surface->w;
    size_t h = surface->h;

    SDL_PixelFormat *format = surface->format;
    Uint32 *pixels = surface->pixels;

    for (size_t i = 0; i < w * h; i++)
        pixels[i] = SDL_MapRGB(format, 0, 0, 0);
}

static void clean_border(SDL_Surface *surface, SDL_Surface *gray)
{
    size_t w = surface->w;
    size_t h = surface->h;

    size_t x1 = w / 2 - 1, x2 = w / 2 + 1, y1 = h / 2 - 1, y2 = h / 2 + 1;
    while (check_square(surface, x1, x2, y1, y2) == 0
           && !(x1 == 0 && x2 == w - 1 && y1 == 0 && y2 == h - 1))
    {
        if (x1 > 0 && check_line_v(surface, x1, y1, y2) == 0)
            x1 -= 1;
        if (x2 < w - 1 && check_line_v(surface, x2, y1, y2) == 0)
            x2 += 1;
        if (y1 > 0 && check_line_h(surface, x1, x2, y1) == 0)
            y1 -= 1;
        if (y2 < h - 1 && check_line_h(surface, x1, x2, y2) == 0)
            y2 += 1;
    }
    // test if the square is smaller than 30% of the image
    if ((x2 - x1) * (y2 - y1) < (w * h) * 0.3)
    {
        int changed = 1;
        while (changed && !(x1 == 0 && x2 == w - 1 && y1 == 0 && y2 == h - 1))
        {
            changed = 0;
            if (x1 > 0 && check_line_v(surface, x1, y1, y2) > 0)
            {
                changed = 1;
                x1 -= 1;
            }
            if (x2 < w - 1 && check_line_v(surface, x2, y1, y2) > 0)
            {
                changed = 1;
                x2 += 1;
            }
            if (y1 > 0 && check_line_h(surface, x1, x2, y1) > 0)
            {
                changed = 1;
                y1 -= 1;
            }
            if (y2 < h - 1 && check_line_h(surface, x1, x2, y2) > 0)
            {
                changed = 1;
                y2 += 1;
            }
        }

        remove_border(surface, x1, x2, y1, y2);
        remove_border(gray, x1, x2, y1, y2);
    }
    else
    {
        picture_black(surface);
        picture_black(gray);
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
    if (w < new_w || h < new_h)
        return;
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

static int is_white(SDL_Surface *surface)
{
    size_t w = surface->w;
    size_t h = surface->h;

    SDL_PixelFormat *format = surface->format;
    Uint32 *pixels = surface->pixels;

    size_t x1 = w / 2 - 1, x2 = w / 2 + 1, y1 = h / 2 - 1, y2 = h / 2 + 1;

    while ((x2 - x1) * (y2 - y1) < (w * h) * 0.3)
    {
        for (size_t i = x1; i < x2; i++)
        {
            Uint8 r, g, b;
            SDL_GetRGB(pixels[y1 * w + i], format, &r, &g, &b);
            if (r + g + b == 0)
                return 0;
            SDL_GetRGB(pixels[y2 * w + i], format, &r, &g, &b);
            if (r + g + b == 0)
                return 0;
        }
        for (size_t j = y1; j < y2; j++)
        {
            Uint8 r, g, b;
            SDL_GetRGB(pixels[j * w + x1], format, &r, &g, &b);
            if (r + g + b == 0)
                return 0;
            SDL_GetRGB(pixels[j * w + x2], format, &r, &g, &b);
            if (r + g + b == 0)
                return 0;
        }

        if (x1 > 0)
            x1 -= 1;
        if (x2 < w - 1)
            x2 += 1;
        if (y1 > 0)
            y1 -= 1;
        if (y2 < h - 1)
            y2 += 1;
    }
    return 1;
}

static void clean_picture(SDL_Surface *b_w, SDL_Surface *gray)
{
    size_t w = b_w->w;
    size_t h = b_w->h;

    size_t x1 = w / 2 - 1, x2 = w / 2 + 1, y1 = h / 2 - 1, y2 = h / 2 + 1;
    while (check_square(b_w, x1, x2, y1, y2) == 0
           && !(x1 == 0 && x2 == w - 1 && y1 == 0 && y2 == h - 1))
    {
        if (x1 > 0 && check_line_v(b_w, x1, y1, y2) == 0)
            x1 -= 1;
        if (x2 < w - 1 && check_line_v(b_w, x2, y1, y2) == 0)
            x2 += 1;
        if (y1 > 0 && check_line_h(b_w, x1, x2, y1) == 0)
            y1 -= 1;
        if (y2 < h - 1 && check_line_h(b_w, x1, x2, y2) == 0)
            y2 += 1;
    }

    if ((x2 - x1) * (y2 - y1) > (w * h) * 0.3 || is_white(b_w))
    {
        picture_black(b_w);
        picture_black(gray);
    }
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
    if (crop(&output_b_w, &output) != 0)
        return 1;
    if (output == NULL)
        return 1;
    if (output->w < 28 || output->h < 28)
    {
        add_border(&output, output->w, output->h,
            (size_t)fmax((double)(28 - output->w), (double)(28 - output->h)));
        add_border(&output_b_w, output_b_w->w, output_b_w->h,
            (size_t)fmax(
                (double)(28 - output_b_w->w), (double)(28 - output_b_w->h)));
    }
    scale_down(&output, output->w, output->h, 20, 20);
    scale_down(&output_b_w, output_b_w->w, output_b_w->h, 20, 20);
    add_border(&output, 20, 20, 4);
    add_border(&output_b_w, 20, 20, 4);
    clean_picture(output_b_w, output);

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
    size_t lines[][2], size_t i, size_t rho, size_t theta, int dim)
{
    for (size_t j = dim; j > i; j--)
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
static void add_middle_lines(size_t lines[][2], int dim)
{
    int root = sqrt(dim);
    for (size_t i = 0; i < (size_t)(2 * (root + 1)); i += root)
    {
        size_t intermediary = (size_t)((lines[i + 1][0] - lines[i][0]) / root);
        for (size_t j = 1; j < (size_t)root; j++)
            insert_line(lines, i + j, lines[i][0] + j * intermediary,
                lines[i][1], dim);
    }
}

// stores lines' coordinates in the "lines" matrix
static int store_lines(matrix_t *acc, size_t rhos, ssize_t lines[][2],
    double maximum, int thres, int dim)
{
    for (size_t i = 0; i < (size_t)(2 * ALL_LINES(dim)); i++)
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
                lines[x][0] = i;
                lines[x][1] = j;
                x++;
            }
        }

    return x;
}

static size_t set_line_coordinates(
    size_t array[][2], size_t i1, size_t i, ssize_t lines[][2])
{
    array[i1][0] = lines[i][0];
    array[i1][1] = lines[i][1];
    return i1 + 1;
}

// separates vertical and horizontal lines + orders them in increasing order
// according to their distance from origin t1, t2 = approximated theta for one
// of the lines' type
static void separate_lines(size_t lines1[][2], size_t lines2[][2], size_t len,
    ssize_t lines[][2], size_t *len1, size_t *len2)
{
    int theta_thres = 10;
    int t1 = -1, t2 = -1;
    size_t i1 = 0, i2 = 0;

    for (size_t i = 0; i < len; i++)
    {
        if (t1 == -1)
        {
            t1 = lines[i][1];
            i1 = set_line_coordinates(lines1, i1, i, lines);
            (*len1)++;
        }
        else
        {
            int lt = (int)lines[i][1] % 180;
            if ((lt > t1 - theta_thres && lt < t1 + theta_thres)
                || (lt > t1 - theta_thres + 180 && lt < t1 + theta_thres + 180)
                || (lt > t1 - theta_thres - 180
                    && lt < t1 + theta_thres - 180))
            {
                i1 = set_line_coordinates(lines1, i1, i, lines);
                (*len1)++;
            }
            else
            {
                if (t2 == -1)
                {
                    t2 = lines[i][1];
                    i2 = set_line_coordinates(lines2, i2, i, lines);
                    (*len2)++;
                }
                else
                {
                    if ((lt > t1 - theta_thres && lt < t1 + theta_thres)
                        || (lt > t1 - theta_thres + 180
                            && lt < t1 + theta_thres + 180)
                        || (lt > t1 - theta_thres - 180
                            && lt < t1 + theta_thres - 180))
                    {
                        i1 = set_line_coordinates(lines1, i1, i, lines);
                        (*len1)++;
                    }
                    else
                    {
                        if ((lt > t2 - theta_thres && lt < t2 + theta_thres)
                            || (lt > t2 - theta_thres + 180
                                && lt < t2 + theta_thres + 180)
                            || (lt > t2 - theta_thres - 180
                                && lt < t2 + theta_thres - 180))
                        {
                            i2 = set_line_coordinates(lines2, i2, i, lines);
                            (*len2)++;
                        }
                    }
                }
            }
        }
    }
    sort_list(lines1, *len1);
    sort_list(lines2, *len2);
}

static double mean(double data[], size_t start, size_t end)
{
    double sum = 0.0;
    for (size_t i = start; i < end; i++)
        sum += data[i];

    return sum / (end - start);
}

void calibrate_line(size_t lines[][2], size_t len)
{
    double distances[len - 1];
    for (size_t i = 1; i < len; i++)
    {
        distances[i - 1]
            = (double)(abs((int)lines[i][0] - (int)lines[i - 1][0]));
    }

    double m1 = mean(distances, 1, len - 1);
    double m2 = mean(distances, 0, len - 2);

    double thres1 = 0.5 * m1;
    double thres2 = 0.5 * m2;

    if (distances[0] < m1 - thres1 || distances[0] > m1 + thres1)
        lines[0][0] = lines[1][0] - m1;
    if (distances[len - 2] < m2 - thres2 || distances[len - 2] > m2 + thres2)
        lines[len - 1][0] = lines[len - 2][0] + m2;
}

void move_left(size_t lines[][2], size_t x, size_t len)
{
    for (size_t i = x; i < len; i++)
    {
        lines[i - 1][0] = lines[i][0];
        lines[i - 1][1] = lines[i][1];
    }
}

size_t clean_lines(size_t lines[][2], size_t len, size_t w, size_t h)
{
    double thetas[len];
    for (size_t i = 0; i < len; i++)
        thetas[i] = (double)lines[i][1]; // modulo((double)lines[i][1], 180);

    double m = mean(thetas, 0, len);
    double thres_r = 30 * sqrt((w * w) + (h * h)) / 1000;

    for (size_t i = 1; i < len; i++)
    {
        if (((lines[i - 1][1]) > m + 5 || (lines[i - 1][1]) < m - 5)
            || abs((int)lines[i - 1][0] - (int)lines[i][0]) < thres_r)
        {
            move_left(lines, i, len);
            len--;
            i--;
        }
    }
    for (size_t i = 0; i < len; i++)
        if (lines[i][1] < m - 2 || lines[i][1] > m + 2)
            lines[i][1] = m;
    return len;
}

static void add_missing_lines(size_t lines[][2], size_t len, int dim)
{
    double distances[len - 1];
    for (size_t i = 1; i < len; i++)
    {
        distances[i - 1]
            = (double)(abs((int)lines[i][0] - (int)lines[i - 1][0]));
    }

    double double_m = 2 * mean(distances, 0, len - 1);

    double thres = 0.3 * double_m;

    for (size_t i = 0; i < len - 1; i++)
    {
        if (distances[i] > double_m - thres && distances[i] < double_m + thres)
        {
            size_t middle = (lines[i + 1][0] - lines[i][0]) / 2 + lines[i][0];
            insert_line(lines, i + 1, middle, lines[i][1], dim);
            i++;
        }
    }
}

static int intersections(matrix_t *acc, size_t rhos, SDL_Surface *surface,
    SDL_Surface *sudoku, double maximum, int thres, char path[30],
    int get_rotation, int dim)
{
    size_t large_num = 6 * dim;
    ssize_t lines[large_num][2];
    int l = store_lines(acc, rhos, lines, maximum, thres, dim);

    if (l < 0)
        return 1;
    size_t len = (size_t)l;
    size_t lines1[large_num / 2][2];
    size_t lines2[large_num / 2][2];
    size_t len1 = 0, len2 = 0;

    separate_lines(lines1, lines2, len, lines, &len1, &len2);
    if (len1 > BIG_LINES(dim) && len1 != (size_t)(ALL_LINES(dim)))
        len1 = clean_lines(lines1, len1, surface->w, surface->h);
    if (len2 > BIG_LINES(dim) && len2 != (size_t)(ALL_LINES(dim)))
        len2 = (clean_lines(lines2, len2, surface->w, surface->h));
    len = len1 + len2;
    calibrate_line(lines1, len1);
    calibrate_line(lines2, len2);

    if (len1 < (size_t)ALL_LINES(dim) && len1 != (size_t)BIG_LINES(dim))
    {
        add_missing_lines(lines1, len1, dim);
        len1++;
    }
    if (len2 < (size_t)ALL_LINES(dim) && len2 != (size_t)BIG_LINES(dim))
    {
        add_missing_lines(lines2, len2, dim);
        len2++;
    }

    if (len == 2 * (BIG_LINES(dim)))
    {
        add_middle_lines(lines1, dim);
        add_middle_lines(lines2, dim);
        len = 2 * ALL_LINES(dim);
    }

    matrix_t *inter = matrix_create((len / 2) * (len / 2), 2, 0);

    mat_transform(inter, (mat_transform_t)set_minus_one, 1);
    draw_line(surface, inter, lines1, len / 2);
    size_t n = draw_line(surface, inter, lines2, len / 2);

    char out[50];
    sprintf(out, "%s/lines.png", OUTPUT_FOLDER);

    IMG_SavePNG(surface, out);

    if (n < (len / 2) * (len / 2))
        return 1;

    if (!get_rotation)
        cut_squares(inter, surface, sudoku, len / 2, path);

    matrix_free(inter);
    return 0;
}

static void get_lens(matrix_t *acc, size_t rhos, double maximum, int thres,
    size_t i, size_t j, int *lenr, int *lent)
{
    double ratio = maximum / 1000;
    int x = i, y = j;
    while (mat_el_at(acc, x, j) / ratio > thres && x < (int)rhos)
        x++;
    while (mat_el_at(acc, i, y) / ratio > thres && y < 360)
        y++;
    *lenr = x;
    *lent = y;
}

static void fuze_lines(matrix_t *acc, size_t rhos, double maximum, int thres)
{
    double ratio = maximum / 1000;
    int del_t = 70; // once i find a line, i delete the ones that are too close
                    // and have an angle that is (theta - 70 < t < theta + 70),
                    // theta being the angle of the line I found and t being
                    // the angle of the line I remove
    int del_r = 20 * rhos / 1000;
    for (size_t j = 0; j < 360; j++)
        for (size_t i = 0; i < rhos; i++)
        {
            double val = mat_el_at(acc, i, j) / ratio;
            if (val > thres) // considered a real line
            {
                int lenr, lent;
                get_lens(acc, rhos, maximum, thres, i, j, &lenr, &lent);
                int midx = ((lenr - i) / 2) + i;
                int midy = ((lent - j) / 2) + j;
                for (int x = midx - del_r; x < midx + del_r; x++)
                {
                    for (int y = midy - del_t; y < midy + del_t; y++)
                    {
                        if (x < (int)rhos
                            && ((x >= 0 && y >= 0) || (x >= 0 && y > 0)))
                        {
                            mat_set_el(acc, x, y % 360, 0);
                            mat_set_el(acc, x, (y + 180) % 360, 0);
                        }
                    }
                }
                mat_set_el(acc, midx, midy, maximum);
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

static double get_max(matrix_t *acc, size_t rhos, int *thres, int dim)
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
    if (count_lines(acc, rhos, maximum, *thres) < dim)
    {
        *thres -= 100;
        return get_max(acc, rhos, thres, dim);
    }
    return maximum;
}

int rotation(matrix_t *acc, size_t rho, double maximum, int dim)
{
    int count = 0;
    double res = 0;
    size_t large_num = 6 * dim;
    ssize_t lines[large_num][2];
    size_t lines1[large_num / 2][2];
    size_t lines2[large_num / 2][2];
    size_t len1 = 0;
    size_t len2 = 0;
    for (size_t t = 0; t < 360; t++)
        for (size_t r = 0; r < rho; r++)
        {
            if (mat_el_at(acc, r, t) == maximum)
            {
                lines[count][0] = (ssize_t)r;
                lines[count][1] = (ssize_t)t;
                count++;
            }
        }
    separate_lines(lines1, lines2, count, lines, &len1, &len2);
    for (size_t i = 0; i < len1; i++)
        res += lines1[i][1] % 180 > 135 ? (double)modulo(lines1[i][1], 180)
                                        : (double)(lines1[i][1] % 180);

    if (len1 == 0)
        return 0;
    return (round(res / len1));
}

void draw_acc(matrix_t *acc, size_t rhos, double max)
{
    SDL_Surface *surface = SDL_CreateRGBSurface(0, 360, rhos, 32, 0, 0, 0, 0);
    SDL_PixelFormat *format = surface->format;
    Uint32 *pixels = surface->pixels;
    for (size_t i = 0; i < rhos; i++)
        for (size_t j = 0; j < 360; j++)
        {
            double val = mat_el_at(acc, i, j) / max;
            pixels[i * surface->w + j] = SDL_MapRGB(format, val * 255, 0, 0);
        }
    char out[50];
    sprintf(out, "%s/hough.png", OUTPUT_FOLDER);

    IMG_SavePNG(surface, out);
}

static int line_detection(SDL_Surface *surface, SDL_Surface *sudoku,
    int get_rotation, char path[30], int dim)
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

            if (r + g + b == WHITE)
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

    double maximum = get_max(acc, diag, &thres, 2 * BIG_LINES(dim));

    int value = intersections(
        acc, diag, surface, sudoku, maximum, thres, path, get_rotation, dim);

    if (get_rotation)
        value = rotation(acc, diag, maximum, dim);

    matrix_free(acc);
    return value;
}

int sudoku_split(char *black_white, char *grayscale, char *path, int dim)
{
    // Initializes the SDL.
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
        return 1;

    SDL_Surface *b_w = load_image(black_white);
    if (b_w == NULL)
        return 1;
    SDL_Surface *gray = load_image(grayscale);
    if (gray == NULL)
        return 1;

    char save_path[30];
    strcpy(save_path, path);

    add_border(&b_w, b_w->w, b_w->h, 10);
    add_border(&gray, gray->w, gray->h, 10);

    int value = line_detection(b_w, gray, 0, save_path, dim);

    SDL_FreeSurface(b_w);
    SDL_FreeSurface(gray);

    SDL_Quit();
    if (value == ERROR_CODE)
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

int get_rotation(SDL_Surface *surface, int dim)
{
    add_border(&surface, surface->w, surface->h, 10);
    char save_path[30] = {0};
    int theta = line_detection(surface, surface, 1, save_path, dim);
    if (theta == ERROR_CODE)
        return theta;
    else
        return modulo(theta + 180, 90);
}
