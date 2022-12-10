#include <stdio.h>
#include "utils.h"

void ui_crop_pixbuf(const GdkPixbuf *origin, double top, double bottom,
    double right, double left, GdkPixbuf **dest)
{

    if (top + bottom >= 1 || right + left >= 1)
        return;

    int height = gdk_pixbuf_get_height(origin);
    int width = gdk_pixbuf_get_width(origin);
    int src_x = left * width;
    int src_y = top * height;
    int new_width = (1 - right - left) * width;
    int new_height = (1 - bottom - top) * height;

    int bits_per_sample = gdk_pixbuf_get_bits_per_sample(origin);
    GdkColorspace color_space = gdk_pixbuf_get_colorspace(origin);
    gboolean has_alpha = gdk_pixbuf_get_has_alpha(origin);

    *dest = gdk_pixbuf_new(
        color_space, has_alpha, bits_per_sample, width, height);
    gdk_pixbuf_copy_area(
        origin, src_x, src_y, new_width, new_height, *dest, src_x, src_y);
}
