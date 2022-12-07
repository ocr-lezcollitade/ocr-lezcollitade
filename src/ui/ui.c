#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <gtk/gtk.h>
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "ui.h"
#include "../network/network.h"
#include "../preproc/color_removal/color_removal.h"
#include "../preproc/rotate/rotate.h"
#include "../preproc/sudoku_detection/sudoku_split.h"
#include "../utils/img_loader/loader.h"

#define GRID_DIM 9
#define UNUSED(x) (void)(x)

/** GLOBAL VARIABLES **/

GtkBuilder *builder;
bool chooser_selected = 0;

/** FUNCTIONS **/

void on_start(GtkWidget *w, gpointer data)
{
    UNUSED(w);
    if (chooser_selected)
    {
        GtkStack *stack = GTK_STACK(gtk_builder_get_object(builder, "stack"));
        GtkWidget *dest = data;
        gtk_stack_set_visible_child(stack, dest);
        load_rotate();
    }
}

void go_to(GtkWidget *w, gpointer data)
{
    UNUSED(w);
    GtkWidget *dest = data;
    GtkStack *stack = GTK_STACK(gtk_builder_get_object(builder, "stack"));
    gtk_stack_set_visible_child(stack, dest);
}

void auto_rotation()
{
    char filename[50];
    snprintf(filename, 50, "%s/current.png", OUTPUT_FOLDER);
    SDL_Surface *current_surface = load_image(filename);
    SDL_Surface *temp = load_image(filename);
    surface_to_grayscale(temp);
    SDL_Surface *binarized_surface = full_binary(temp);
    int rotation = get_rotation(binarized_surface, GRID_DIM);
    SDL_Surface *rotated = rotate_surface(current_surface, -rotation);
    snprintf(filename, 50, "%s/rotated.png", OUTPUT_FOLDER);
    IMG_SavePNG(rotated, filename);

    SDL_FreeSurface(temp);
    SDL_FreeSurface(rotated);

    load_grayscale();
}

void load_binary()
{
    char filename[50];
    snprintf(filename, 50, "%s/grayscaled.png", OUTPUT_FOLDER);
    SDL_Surface *temp_surface = load_image(filename);
    SDL_Surface *bs = full_binary(temp_surface);
    snprintf(filename, 50, "%s/binarized.png", OUTPUT_FOLDER);
    IMG_SavePNG(bs, filename);

    scale_down(&bs, bs->w, bs->h, (int)(bs->w / (bs->h / 500.0)), 500);
    snprintf(filename, 50, "%s/tiny_binarized.png", OUTPUT_FOLDER);
    IMG_SavePNG(bs, filename);

    GtkImage *image
        = GTK_IMAGE(gtk_builder_get_object(builder, "binarized_image"));
    gtk_image_set_from_file(image, filename);

    SDL_FreeSurface(temp_surface);
    SDL_FreeSurface(bs);
}

void load_grayscale()
{
    SDL_Surface *gs;

    char rotated[50], tiny_grayscaled[50], filename[50];
    snprintf(rotated, 50, "%s/rotated.png", OUTPUT_FOLDER);
    snprintf(tiny_grayscaled, 50, "%s/tiny_grayscaled.png", OUTPUT_FOLDER);
    snprintf(filename, 50, "%s/current.png", OUTPUT_FOLDER);

    FILE *file = fopen(rotated, "r");

    if (file == NULL)
        gs = load_image(filename);
    else
    {
        fclose(file);
        gs = load_image(rotated);
    }

    surface_to_grayscale(gs);
    snprintf(filename, 50, "%s/grayscaled.png", OUTPUT_FOLDER);
    IMG_SavePNG(gs, filename);

    scale_down(&gs, gs->w, gs->h, (int)(gs->w / (gs->h / 500.0)), 500);
    IMG_SavePNG(gs, tiny_grayscaled);

    GtkImage *image
        = GTK_IMAGE(gtk_builder_get_object(builder, "grayscaled_image"));
    gtk_image_set_from_file(image, tiny_grayscaled);

    SDL_FreeSurface(gs);
}

void load_rotate()
{
    char current[50], tiny_current[50];
    snprintf(current, 50, "%s/current.png", OUTPUT_FOLDER);
    snprintf(tiny_current, 50, "%s/tiny_current.png", OUTPUT_FOLDER);

    SDL_Surface *cs = load_image(current);
    scale_down(&cs, cs->w, cs->h, (int)(cs->w / (cs->h / 500.0)), 500);
    IMG_SavePNG(cs, tiny_current);

    GtkImage *image
        = GTK_IMAGE(gtk_builder_get_object(builder, "rotated_image"));
    gtk_image_set_from_file(image, tiny_current);

    GtkLabel *label
        = GTK_LABEL(gtk_builder_get_object(builder, "show_rotation"));
    gtk_label_set_text(label, "0");

    GtkRange *range = GTK_RANGE(gtk_builder_get_object(builder, "scrollbar"));
    gtk_range_set_value(range, (double)0);

    SDL_FreeSurface(cs);
}

void load_split()
{
    GtkLabel *label
        = GTK_LABEL(gtk_builder_get_object(builder, "split_result"));

    GtkGrid *grid = GTK_GRID(gtk_builder_get_object(builder, "split_grid"));

    char bin[50], gscaled[50], split_path[50];
    snprintf(bin, 50, "%s/binarized.png", OUTPUT_FOLDER);
    snprintf(gscaled, 50, "%s/grayscaled.png", OUTPUT_FOLDER);
    snprintf(split_path, 50, "%s/split/", OUTPUT_FOLDER);
    mkdir(split_path, 0777);
    if (!sudoku_split(bin, gscaled, split_path, GRID_DIM))
    {
        gtk_widget_show(GTK_WIDGET(grid));

        for (int i = 0; i < 9; i++)
        {
            for (int j = 0; j < 9; j++)
            {
                GtkImage *image = GTK_IMAGE(gtk_grid_get_child_at(grid, i, j));

                char split_no[70];
                snprintf(split_no, 70, "%s%i.png", split_path, i + j * 9);

                gtk_image_set_from_file(image, (gchar *)split_no);
            }
        }

        gtk_label_set_text(label, "The image has been split.");
    }
    else
    {
        gtk_widget_hide(GTK_WIDGET(grid));
        gtk_label_set_text(
            label, "Splitting failed. Please restart with another image.");
    }
}

void on_scrollbar_value_changed(GtkRange *r)
{
    gdouble x = gtk_range_get_value(r);

    char rot_label[5];
    snprintf(rot_label, 5, "%i", (int)x);

    GtkLabel *label
        = GTK_LABEL(gtk_builder_get_object(builder, "show_rotation"));
    gtk_label_set_text(label, (const gchar *)rot_label);

    char filename[50];
    snprintf(filename, 50, "%s/current.png", OUTPUT_FOLDER);
    SDL_Surface *cs = load_image(filename);
    SDL_Surface *rs = rotate_surface(cs, (int)x);
    snprintf(filename, 50, "%s/rotated.png", OUTPUT_FOLDER);
    IMG_SavePNG(rs, filename);

    snprintf(filename, 50, "%s/tiny_current.png", OUTPUT_FOLDER);
    SDL_Surface *tiny_cs = load_image(filename);
    SDL_Surface *tiny_rs = rotate_surface(tiny_cs, (int)x);
    snprintf(filename, 50, "%s/tiny_rotated.png", OUTPUT_FOLDER);
    IMG_SavePNG(tiny_rs, filename);

    GtkImage *image
        = GTK_IMAGE(gtk_builder_get_object(builder, "rotated_image"));
    snprintf(filename, 50, "%s/tiny_rotated.png", OUTPUT_FOLDER);
    gtk_image_set_from_file(image, (const gchar *)filename);

    SDL_FreeSurface(rs);
}

void on_import_file_set(GtkFileChooserButton *f)
{
    char file_name[50];
    snprintf(file_name, 50, "%s/current.png", OUTPUT_FOLDER);

    mkdir(OUTPUT_FOLDER, 0755);

    FILE *source, *target;
    source = fopen(gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(f)), "rb");

    fseek(source, 0, SEEK_END);
    int length = ftell(source);

    fseek(source, 0, SEEK_SET);
    target = fopen(file_name, "wb");

    for (int k = 0; k < length; k++)
        fputc(fgetc(source), target);

    fclose(source);
    fclose(target);

    GtkLabel *label
        = GTK_LABEL(gtk_builder_get_object(builder, "chooser_status"));
    chooser_selected = 1;
    gtk_label_set_text(label, (const gchar *)"A file has been selected!");
}

void deselect()
{
    GtkLabel *label
        = GTK_LABEL(gtk_builder_get_object(builder, "chooser_status"));
    chooser_selected = 0;
    gtk_label_set_text(label, (const gchar *)"No selected image yet");
}

void quit()
{
    clean_directory();
    GtkWidget *window = GTK_WIDGET(gtk_builder_get_object(builder, "window"));
    gtk_window_close(GTK_WINDOW(window));
}

void clean_directory()
{
    recursive_rmdir(OUTPUT_FOLDER);
}

int recursive_rmdir(char *path)
{
    DIR *d = opendir(path);
    size_t path_len = strlen(path);
    int r = -1;

    if (d)
    {
        struct dirent *p;

        r = 0;
        while (!r && (p = readdir(d)))
        {
            int r2 = -1;
            char *buf;
            size_t len;

            if (!strcmp(p->d_name, ".") || !strcmp(p->d_name, ".."))
                continue;

            len = path_len + strlen(p->d_name) + 2;
            buf = malloc(len);

            if (buf)
            {
                struct stat statbuf;

                snprintf(buf, len, "%s/%s", path, p->d_name);
                if (!stat(buf, &statbuf))
                {
                    if (S_ISDIR(statbuf.st_mode))
                        r2 = recursive_rmdir(buf);
                    else
                        r2 = unlink(buf);
                }
                free(buf);
            }
            r = r2;
        }
        closedir(d);
    }

    if (!r)
        r = rmdir(path);

    return r;
}

void open_ui()
{
    gtk_init(0, NULL);

    builder = gtk_builder_new_from_file("resources/ui/ui.glade");

    GtkWidget *window = GTK_WIDGET(gtk_builder_get_object(builder, "window"));

    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    g_signal_connect(window, "destroy", G_CALLBACK(quit), NULL);

    gtk_builder_connect_signals(builder, NULL);

    GtkCssProvider *cssProvider = gtk_css_provider_new();

    gtk_css_provider_load_from_path(cssProvider, "resources/ui/gtk.css", NULL);

    GdkScreen *screen = gdk_screen_get_default();
    gtk_style_context_add_provider_for_screen(screen,
        GTK_STYLE_PROVIDER(cssProvider), GTK_STYLE_PROVIDER_PRIORITY_USER);

    gtk_widget_show(window);

    gtk_main();
}
