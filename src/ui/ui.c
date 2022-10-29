#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <gtk/gtk.h>
#include <gtk/gtkx.h>
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "ui.h"
#include "../preproc/color_removal/color_removal.h"
#include "../preproc/rotate/rotate.h"
#include "../preproc/sudoku_detection/sudoku_split.h"
#include "../utils/img_loader/loader.h"

#define UNUSED(x) (void)(x)

/** GLOBAL VARIABLES **/

GtkWidget *window;
GtkWidget *import;
GtkStack *stack;
GtkImage *rotated_image;
GtkImage *grayscaled_image;
GtkImage *binarized_image;
GtkWidget *scrollbar;
GtkLabel *show_rotation;
GtkLabel *chooser_status;
GtkGrid *split_grid;

void css_set(GtkCssProvider *, GtkWidget *);

GtkAdjustment *adjustment1;

GtkBuilder *builder;

int rotation = 0;
bool chooser_selected = 0;

/** FUNCTIONS **/

void on_start(GtkWidget *w, gpointer data)
{
    UNUSED(w);
    if (chooser_selected)
    {
        GtkWidget *dest = data;
        gtk_stack_set_visible_child(stack, dest);
    }
}

void go_to(GtkWidget *w, gpointer data)
{
    UNUSED(w);
    GtkWidget *dest = data;
    gtk_stack_set_visible_child(stack, dest);
}

void load_binary()
{
    SDL_Surface *temp_surface = load_image("resources/ui/grayscaled.png");

    SDL_Surface *binarized_surface = full_binary(temp_surface);

    IMG_SavePNG(binarized_surface, "resources/ui/binarized.png");

    gtk_image_set_from_file(binarized_image, "resources/ui/binarized.png");

    SDL_FreeSurface(temp_surface);
    SDL_FreeSurface(binarized_surface);
}

void load_grayscale()
{
    SDL_Surface *grayscaled_surface;

    if (rotation == 0)
        grayscaled_surface = load_image("resources/ui/current.png");
    else
        grayscaled_surface = load_image("resources/ui/rotated.png");

    surface_to_grayscale(grayscaled_surface);
    IMG_SavePNG(grayscaled_surface, "resources/ui/grayscaled.png");

    gtk_image_set_from_file(grayscaled_image, "resources/ui/grayscaled.png");

    SDL_FreeSurface(grayscaled_surface);
}

void load_rotate()
{
    char path[] = "resources/ui/current.png";

    gtk_image_set_from_file(rotated_image, path);
}

void load_split()
{
    sudoku_split("resources/ui/binarized.png", "resources/ui/grayscaled.png",
        "resources/ui/");

    for (int i = 0; i < 9; i++)
    {
        for (int j = 0; j < 9; j++)
        {
            GtkImage *image
                = GTK_IMAGE(gtk_grid_get_child_at(split_grid, i, j));

            char split_no[8];
            sprintf(split_no, "%i.png", i + j * 9);

            char full_split[30] = "resources/ui/";
            strcat(full_split, split_no);

            gtk_image_set_from_file(image, (gchar *)full_split);
        }
    }
}

void on_scrollbar_value_changed(GtkRange *r)
{
    gdouble x = gtk_range_get_value(r);
    rotation = (int)x;

    char rot_label[5];
    sprintf(rot_label, "%i", rotation);
    gtk_label_set_text(show_rotation, (const gchar *)rot_label);

    SDL_Surface *rotated_surface = load_image("resources/ui/current.png");
    IMG_SavePNG(
        rotate_surface(rotated_surface, rotation), "resources/ui/rotated.png");

    gtk_image_set_from_file(
        rotated_image, (const gchar *)"resources/ui/rotated.png");

    SDL_FreeSurface(rotated_surface);
}

void on_import_file_set(GtkFileChooserButton *f)
{
    char source_file[strlen(
        gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(f)))];
    strcpy(source_file, gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(f)));

    char file_name[] = "resources/ui/current.png";

    FILE *source, *target;
    source = fopen(source_file, "rb");

    fseek(source, 0, SEEK_END);
    int length = ftell(source);

    fseek(source, 0, SEEK_SET);
    target = fopen(file_name, "wb");

    for (int k = 0; k < length; k++)
        fputc(fgetc(source), target);

    fclose(source);
    fclose(target);

    chooser_selected = 1;
    gtk_label_set_text(
        chooser_status, (const gchar *)"A file has been selected!");
}

void quit()
{
    gtk_window_close(GTK_WINDOW(window));
}

void open_ui()
{
    gtk_init(0, NULL);

    builder = gtk_builder_new_from_file("resources/ui/ui.glade");

    window = GTK_WIDGET(gtk_builder_get_object(builder, "window"));
    import = GTK_WIDGET(gtk_builder_get_object(builder, "import"));
    rotated_image
        = GTK_IMAGE(gtk_builder_get_object(builder, "rotated_image"));
    grayscaled_image
        = GTK_IMAGE(gtk_builder_get_object(builder, "grayscaled_image"));
    binarized_image
        = GTK_IMAGE(gtk_builder_get_object(builder, "binarized_image"));
    scrollbar = GTK_WIDGET(gtk_builder_get_object(builder, "scrollbar"));
    stack = GTK_STACK(gtk_builder_get_object(builder, "stack"));
    show_rotation
        = GTK_LABEL(gtk_builder_get_object(builder, "show_rotation"));
    chooser_status
        = GTK_LABEL(gtk_builder_get_object(builder, "chooser_status"));
    split_grid = GTK_GRID(gtk_builder_get_object(builder, "split_grid"));

    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    gtk_builder_connect_signals(builder, NULL);

    GtkCssProvider *cssProvider = gtk_css_provider_new();

    gtk_css_provider_load_from_path(cssProvider, "resources/ui/gtk.css", NULL);

    GdkScreen *screen = gdk_screen_get_default();
    gtk_style_context_add_provider_for_screen(screen,
        GTK_STYLE_PROVIDER(cssProvider), GTK_STYLE_PROVIDER_PRIORITY_USER);

    gtk_widget_show(window);

    gtk_main();
}
