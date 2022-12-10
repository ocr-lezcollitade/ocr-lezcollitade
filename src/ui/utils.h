#ifndef LEZCOLLITADE_UI_UTILS_H
#define LEZCOLLITADE_UI_UTILS_H

#include <gtk/gtk.h>

/**
 *  \brief  Crops a Pixbuf to given dimensions.
 *  \fn     void ui_crop_pixbuf(const GdkPixbuf *origin, double top, double
 * bottom, double right, double left, GdkPixbuf **res) \param  origin The
 * origin pixbuf to crop from. \param  top A float [0;1] representing the crop
 * ratio from the top. \param  bottom A float [0;1] representing the crop ratio
 * from the bottom. \param  right A float [0;1] representing the crop ratio
 * from the right. \param  left A float [0;1] representing the crop ratio from
 * the left. \param  res A pointer to the GdkPixbuf* result.
 */
void ui_crop_pixbuf(const GdkPixbuf *origin, double top, double bottom,
    double right, double left, GdkPixbuf **res);

#endif /* !LEZCOLLITADE_UI_UTILS_H */
