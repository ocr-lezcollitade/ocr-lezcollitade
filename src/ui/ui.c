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
#include "../preproc/crop/crop.h"
#include "../preproc/rotate/rotate.h"
#include "../preproc/sudoku_detection/sudoku_split.h"
#include "../solver/solver.h"
#include "../utils/img_loader/loader.h"
#include "drawerizer.h"
#include "utils.h"

#define UNUSED(x) (void)(x)

#define UP 0
#define DOWN 1
#define RIGHT 2
#define LEFT 3

/** FUNCTIONS **/
void ui_copy_file(const char *filename)
{
    mkdir(OUTPUT_FOLDER, 0755);

    char output[50];
    snprintf(output, 50, "%s/current.png", OUTPUT_FOLDER);

    // removes the output file to avoid corruption
    remove(output);

    FILE *source = NULL, *target = NULL;

    source = fopen(filename, "rb");

    fseek(source, 0, SEEK_END);
    int length = ftell(source);

    fseek(source, 0, SEEK_SET);
    target = fopen(output, "wb");

    for (int k = 0; k < length; k++)
        fputc(fgetc(source), target);

    fclose(source);
    fclose(target);
}

static void on_start(GtkButton *b, gpointer user_data)
{
    UNUSED(b);
    ui_t *ui = user_data;

    GtkFileChooser *chooser = NULL;
    GtkContainer *file_container = GTK_CONTAINER(
        gtk_builder_get_object(ui->builder, "upload_file_container"));
    GList *children = gtk_container_get_children(file_container);
    for (size_t i = 0; i < g_list_length(children); i++)
    {
        gpointer data = g_list_nth_data(children, i);
        if (GTK_IS_FILE_CHOOSER(data))
        {
            chooser = GTK_FILE_CHOOSER(data);
            break;
        }
    }
    if (chooser == NULL)
        return;

    gchar *filename = gtk_file_chooser_get_filename(chooser);
    if (filename == NULL)
        return;

    ui_copy_file(filename);

    ui_next_frame(ui);
}

static void on_grid_size_changed(GtkToggleButton *button, ui_t *ui)
{
    UNUSED(button);

    GtkToggleButton *button16 = GTK_TOGGLE_BUTTON(
        gtk_builder_get_object(ui->builder, "radiobutton16"));

    if (gtk_toggle_button_get_active(button16))
        ui->process.grid_size = 16;
    else
        ui->process.grid_size = 9;
}

void on_scale_value_changed(GtkRange *r, gpointer user_data)
{
    ui_t *ui = user_data;

    gdouble x = gtk_range_get_value(r);

    char rot_label[5];
    snprintf(rot_label, 5, "%i", (int)x);

    GtkLabel *label
        = GTK_LABEL(gtk_builder_get_object(ui->builder, "show_rotation"));
    gtk_label_set_text(label, (const gchar *)rot_label);

    char filename[50], current[50];
    snprintf(current, 50, "%s/current.png", OUTPUT_FOLDER);
    SDL_Surface *cs = load_image(current);
    SDL_Surface *rs = rotate_surface(cs, (int)x);
    snprintf(filename, 50, "%s/rotated.png", OUTPUT_FOLDER);
    IMG_SavePNG(rs, filename);

    GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file(filename, NULL);
    GdkPixbuf *smol_pixbuf
        = gdk_pixbuf_scale_simple(pixbuf, 600, 600, GDK_INTERP_BILINEAR);

    GtkImage *image
        = GTK_IMAGE(gtk_builder_get_object(ui->builder, "rotate_image"));
    gtk_image_set_from_pixbuf(image, smol_pixbuf);

    SDL_FreeSurface(rs);
}

void on_validate(GtkButton *b, gpointer user_data)
{
    UNUSED(b);
    ui_t *ui = user_data;

    GtkGrid *grid
        = GTK_GRID(gtk_builder_get_object(ui->builder, "results_grid"));

    int *new_grid
        = calloc(ui->process.grid_size * ui->process.grid_size, sizeof(int));

    for (int i = 0; i < ui->process.grid_size; i++)
    {
        for (int j = 0; j < ui->process.grid_size; j++)
        {
            GtkWidget *child = gtk_grid_get_child_at(grid, j, i);

            const gchar *text = gtk_entry_get_text(GTK_ENTRY(child));

            int n;
            if (text[0] >= 'A' && text[0] <= 'Z')
                n = text[0] - 55;
            else
                n = atoi(text);

            new_grid[i * ui->process.grid_size + j] = n;
        }
    }

    int *original = copy_grid(new_grid,
        ui->process.net->layers[ui->process.net->layer_count - 1]->count - 1);
    if (!solve(new_grid,
            ui->process.net->layers[ui->process.net->layer_count - 1]->count
                - 1))
    {
        GtkWidget *popup = gtk_message_dialog_new_with_markup(ui->window,
            GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_INFO,
            GTK_BUTTONS_CLOSE, "Error in the intial grid");
        gtk_window_set_title(GTK_WINDOW(popup), "Error");
        gtk_dialog_run(GTK_DIALOG(popup));
        gtk_widget_destroy(popup);
        return;
    }

    save_sudoku(new_grid, original, ui->process.grid_size);

    free(original);
    free(new_grid);
    ui_next_frame(ui);
}

static void on_quit_w(GtkWindow *w, gpointer user_data)
{
    UNUSED(w);

    ui_t *ui = user_data;

    recursive_rmdir(OUTPUT_FOLDER);

    if (ui->process.net != NULL)
    {
        network_free(ui->process.net);
        ui->process.net = NULL;
    }
}

static void on_quit_b(GtkButton *b, gpointer user_data)
{
    UNUSED(b);

    ui_t *ui = user_data;

    recursive_rmdir(OUTPUT_FOLDER);

    if (ui->process.net != NULL)
    {
        network_free(ui->process.net);
        ui->process.net = NULL;
    }
    gtk_window_close(ui->window);
}

static void on_auto_rotation(GtkButton *b, gpointer user_data)
{
    UNUSED(b);
    ui_t *ui = user_data;

    ui->process.auto_rotate = 1;

    auto_rotation(b, user_data);
}

void auto_rotation(GtkButton *b, gpointer user_data)
{
    UNUSED(b);
    ui_t *ui = user_data;

    char rfile[50], filename[50];
    snprintf(rfile, 50, "%s/rotated.png", OUTPUT_FOLDER);

    FILE *file = fopen(rfile, "r");

    if (ui->process.auto_rotate || file == NULL)
        snprintf(filename, 50, "%s/current.png", OUTPUT_FOLDER);
    else
        snprintf(filename, 50, "%s/rotated.png", OUTPUT_FOLDER);

    SDL_Surface *current_surface = load_image(filename);
    SDL_Surface *temp = load_image(filename);
    surface_to_grayscale(temp);
    SDL_Surface *binarized_surface = full_binary(temp);
    int rotation = get_rotation(binarized_surface, ui->process.grid_size);
    snprintf(filename, 50, "%s/rotated.png", OUTPUT_FOLDER);
    if (rotation != ERROR_CODE_SUDOKU_SPLIT)
    {
        SDL_Surface *rotated = rotate_surface(current_surface, -rotation);
        IMG_SavePNG(rotated, filename);
        SDL_FreeSurface(rotated);
    }
    else
        IMG_SavePNG(current_surface, filename);

    SDL_FreeSurface(temp);

    ui_next_frame(ui);
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

ui_frame_t ui_get_current_frame(ui_t *ui)
{
    return ui->frames[ui->frame_index];
}

void ui_free(ui_t *ui)
{
    free(ui->frames);
    free(ui);
}

static GtkContainer *ui_get_frame_by_id(ui_t *ui, const char *name)
{
    return GTK_CONTAINER(gtk_builder_get_object(ui->builder, name));
}

static guint container_get_children(GtkContainer *container)
{
    GList *children = gtk_container_get_children(container);
    guint count = g_list_length(children);
    g_list_free(children);
    return count;
}

void ui_go_to(ui_t *ui, size_t index)
{
    if (index >= ui->frame_count)
        return;
    ui_frame_t old_frame = ui_get_current_frame(ui);
    ui->frame_index = index;
    ui_frame_t frame = ui_get_current_frame(ui);

    gtk_stack_set_visible_child(ui->stack, GTK_WIDGET(frame.frame));
    if (frame.on_load != NULL)
        frame.on_load(ui);
    if (old_frame.on_unload != NULL)
        old_frame.on_unload(ui);
}

void ui_next_frame(ui_t *ui)
{
    ui_go_to(ui, ui->frame_index + 1);
}

void ui_restart(ui_t *ui)
{
    ui_go_to(ui, UPLOAD_FRAME);
    recursive_rmdir(OUTPUT_FOLDER);
    if (ui->process.net != NULL)
        network_free(ui->process.net);

    if (ui->process.grid_size == 9)
        ui->process.net = network_load(NET_MAP_9);
    else
        ui->process.net = network_load(NET_MAP_16);
}

static void on_ui_restart(GtkButton *button, gpointer user_data)
{
    UNUSED(button);
    ui_t *ui = user_data;
    ui_restart(ui);
}

ui_t *build_ui(GtkBuilder *builder)
{
    ui_t *ui = malloc(sizeof(ui_t));
    if (ui == NULL)
        return NULL;
    ui->window = GTK_WINDOW(gtk_builder_get_object(builder, "window"));
    ui->stack = GTK_STACK(gtk_builder_get_object(builder, "stack"));
    ui->builder = builder;
    guint ccount = container_get_children(GTK_CONTAINER(ui->stack));
    ui->frames = calloc(ccount, sizeof(ui_frame_t));
    ui->frame_index = 0;
    ui->process
        = (struct process) {.grid_size = 9, .auto_rotate = 0, .net = NULL};
    if (ui->frames == NULL)
        return NULL;
    return ui;
}

static void on_next_clicked(GtkButton *button, gpointer user_data)
{
    UNUSED(button);
    ui_t *ui = user_data;
    ui_next_frame(ui);
}

// upload frame
static void on_ui_file_changed(
    GtkFileChooserButton *chooser, gpointer user_data)
{
    UNUSED(chooser);
    ui_t *ui = user_data;
    GtkLabel *label
        = GTK_LABEL(gtk_builder_get_object(ui->builder, "chooser_status"));
    gtk_label_set_text(label, (const gchar *)"A file has been selected!");
}

static void on_ui_load_upload(ui_t *ui)
{
    GtkBox *file_container = GTK_BOX(
        gtk_builder_get_object(ui->builder, "upload_file_container"));

    GList *children
        = gtk_container_get_children(GTK_CONTAINER(file_container));
    for (size_t i = 0; i < g_list_length(children); i++)
    {
        GtkWidget *w = g_list_nth_data(children, i);
        gtk_container_remove(GTK_CONTAINER(file_container), w);
    }

    GtkWidget *fcb = gtk_file_chooser_button_new(
        "pick a file", GTK_FILE_CHOOSER_ACTION_OPEN);
    gtk_box_pack_start(file_container, fcb, TRUE, TRUE, 0);
    gtk_widget_show(fcb);
    g_signal_connect(GTK_FILE_CHOOSER_BUTTON(fcb), "file-set",
        G_CALLBACK(on_ui_file_changed), ui);
}

static void on_ui_unload_upload(ui_t *ui)
{
    ui_frame_t frame = ui->frames[UPLOAD_FRAME];

    UNUSED(frame);

    GtkLabel *l
        = GTK_LABEL(gtk_builder_get_object(ui->builder, "chooser_status"));
    gtk_label_set_text(l, (const gchar *)"No selected image yet");

    if (ui->process.grid_size == 9)
        ui->process.net = network_load(NET_MAP_9);
    else
        ui->process.net = network_load(NET_MAP_16);
}

static void ui_load_upload(ui_t *ui)
{
    GtkContainer *w = ui_get_frame_by_id(ui, "upload");
    ui_frame_t res = (ui_frame_t) {.frame = w,
        .on_load = on_ui_load_upload,
        .on_unload = on_ui_unload_upload};
    ui->frames[UPLOAD_FRAME] = res;

    GtkToggleButton *tb9 = GTK_TOGGLE_BUTTON(
        gtk_builder_get_object(ui->builder, "radiobutton9"));
    g_signal_connect(tb9, "toggled", G_CALLBACK(on_grid_size_changed), ui);

    GtkToggleButton *tb16 = GTK_TOGGLE_BUTTON(
        gtk_builder_get_object(ui->builder, "radiobutton16"));
    g_signal_connect(tb16, "toggled", G_CALLBACK(on_grid_size_changed), ui);

    GtkButton *button
        = GTK_BUTTON(gtk_builder_get_object(ui->builder, "start"));
    g_signal_connect(button, "clicked", G_CALLBACK(on_start), ui);
}

static void ui_resize_crop_image(ui_t *ui)
{
    double ratios[4];
    char *names[] = {"up", "down", "right", "left"};
    char name[30];
    for (size_t i = 0; i < 4; i++)
    {
        snprintf(name, 30, "resize_scale_%s", names[i]);
        GtkRange *r = GTK_RANGE(gtk_builder_get_object(ui->builder, name));
        GtkAdjustment *adj = gtk_range_get_adjustment(r);
        gdouble value = gtk_adjustment_get_value(adj);
        ratios[i] = value / 100;
    }

    GdkPixbuf *cropped = NULL;
    ui_crop_pixbuf(ui->process.resize_pixbuf, ratios[UP], ratios[DOWN],
        ratios[RIGHT], ratios[LEFT], &cropped);
    if (cropped == NULL)
        return;

    GtkImage *image
        = GTK_IMAGE(gtk_builder_get_object(ui->builder, "resize_image"));
    gtk_image_set_from_pixbuf(image, cropped);
}

static void resize_crop_image(ui_t *ui)
{
    double ratios[4];
    char *names[] = {"up", "down", "right", "left"};
    char name[30];
    for (size_t i = 0; i < 4; i++)
    {
        snprintf(name, 30, "resize_scale_%s", names[i]);
        GtkRange *r = GTK_RANGE(gtk_builder_get_object(ui->builder, name));
        GtkAdjustment *adj = gtk_range_get_adjustment(r);
        gdouble value = gtk_adjustment_get_value(adj);
        ratios[i] = value / 100;
    }

    char filename[50], resized[50];
    snprintf(filename, 50, "%s/rotated.png", OUTPUT_FOLDER);
    snprintf(resized, 50, "%s/resized.png", OUTPUT_FOLDER);
    crop_image(filename, ratios[LEFT], ratios[UP], ratios[RIGHT], ratios[DOWN],
        resized);
}

// resize frame
static void resize_scroll_on_value_change(GtkRange *r, gpointer user_data)
{
    struct ui_user_data *data = user_data;
    ui_t *ui = data->ui;
    UNUSED(r);
    ui_resize_crop_image(ui);
}

static void on_ui_load_resize(ui_t *ui)
{
    char filename[50];
    snprintf(filename, 50, "%s/rotated.png", OUTPUT_FOLDER);

    GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file(filename, NULL);
    GdkPixbuf *smol_pixbuf
        = gdk_pixbuf_scale_simple(pixbuf, 600, 600, GDK_INTERP_BILINEAR);

    GtkImage *image
        = GTK_IMAGE(gtk_builder_get_object(ui->builder, "resize_image"));
    gtk_image_set_from_pixbuf(image, smol_pixbuf);

    ui->process.resize_pixbuf = smol_pixbuf;

    GtkRange *range
        = GTK_RANGE(gtk_builder_get_object(ui->builder, "resize_scale_up"));
    gtk_range_set_value(range, (double)0);
    range
        = GTK_RANGE(gtk_builder_get_object(ui->builder, "resize_scale_down"));
    gtk_range_set_value(range, (double)0);
    range
        = GTK_RANGE(gtk_builder_get_object(ui->builder, "resize_scale_left"));
    gtk_range_set_value(range, (double)0);
    range
        = GTK_RANGE(gtk_builder_get_object(ui->builder, "resize_scale_right"));
    gtk_range_set_value(range, (double)0);
}

static void on_ui_unload_resize(ui_t *ui)
{
    ui_frame_t frame = ui->frames[UPLOAD_FRAME];
    UNUSED(frame);
    GtkImage *image
        = GTK_IMAGE(gtk_builder_get_object(ui->builder, "resize_image"));
    gtk_image_clear(image);
}

static void ui_load_resize(ui_t *ui, struct ui_user_data ***list, size_t *len)
{
    size_t size = *len;
    *list = realloc(*list, (size + 5) * sizeof(struct ui_user_data *));
    if (*list == NULL)
    {
        *len = 0;
        return;
    }

    GtkContainer *w = ui_get_frame_by_id(ui, "resize");
    ui_frame_t res = (ui_frame_t) {.frame = w,
        .on_load = on_ui_load_resize,
        .on_unload = on_ui_unload_resize};
    ui->frames[RESIZE_FRAME] = res;

    const char *names[4] = {"up", "down", "right", "left"};
    char name[30];
    for (size_t i = 0; i < 4; i++)
    {
        snprintf(name, 30, "resize_scale_%s", names[i]);
        GtkRange *range = GTK_RANGE(gtk_builder_get_object(ui->builder, name));
        struct ui_user_data *d = malloc(sizeof(struct ui_user_data));
        *d = (struct ui_user_data) {.ui = ui, .user_data = (gpointer)i};
        (*list)[size++] = d;
        g_signal_connect(range, "value-changed",
            G_CALLBACK(resize_scroll_on_value_change), d);
    }

    GtkButton *button
        = GTK_BUTTON(gtk_builder_get_object(ui->builder, "resize_next"));
    g_signal_connect(button, "clicked", G_CALLBACK(on_next_clicked), ui);
    button = GTK_BUTTON(gtk_builder_get_object(ui->builder, "restart_resize"));
    g_signal_connect(button, "clicked", G_CALLBACK(on_ui_restart), ui);

    *len = size;
}

// rotate frame
static void on_ui_load_rotate(ui_t *ui)
{
    char current[50];
    snprintf(current, 50, "%s/current.png", OUTPUT_FOLDER);
    GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file(current, NULL);
    GdkPixbuf *smol_pixbuf
        = gdk_pixbuf_scale_simple(pixbuf, 600, 600, GDK_INTERP_BILINEAR);

    GtkImage *image
        = GTK_IMAGE(gtk_builder_get_object(ui->builder, "rotate_image"));
    gtk_image_set_from_pixbuf(image, smol_pixbuf);

    GtkLabel *label
        = GTK_LABEL(gtk_builder_get_object(ui->builder, "show_rotation"));
    gtk_label_set_text(label, "0");

    GtkRange *range
        = GTK_RANGE(gtk_builder_get_object(ui->builder, "rotate_scale"));
    gtk_range_set_value(range, (double)0);
}

static void on_ui_unload_rotate(ui_t *ui)
{
    GtkImage *image
        = GTK_IMAGE(gtk_builder_get_object(ui->builder, "rotate_image"));
    gtk_image_clear(image);
}

static void ui_load_rotate(ui_t *ui)
{

    GtkContainer *w = ui_get_frame_by_id(ui, "rotate");
    ui_frame_t res = (ui_frame_t) {.frame = w,
        .on_load = on_ui_load_rotate,
        .on_unload = on_ui_unload_rotate};
    ui->frames[ROTATE_FRAME] = res;

    GtkButton *to_gray
        = GTK_BUTTON(gtk_builder_get_object(ui->builder, "go_grayscale"));
    g_signal_connect(to_gray, "clicked", G_CALLBACK(auto_rotation), ui);

    GtkButton *auto_rotate
        = GTK_BUTTON(gtk_builder_get_object(ui->builder, "auto_rotate"));
    g_signal_connect(auto_rotate, "clicked", G_CALLBACK(on_auto_rotation), ui);

    GtkButton *restart
        = GTK_BUTTON(gtk_builder_get_object(ui->builder, "restart_rotate"));
    g_signal_connect(restart, "clicked", G_CALLBACK(on_ui_restart), ui);

    GtkRange *range
        = GTK_RANGE(gtk_builder_get_object(ui->builder, "rotate_scale"));
    g_signal_connect(
        range, "value-changed", G_CALLBACK(on_scale_value_changed), ui);
}

// grayscale frame
static void on_ui_load_grayscale(ui_t *ui)
{
    resize_crop_image(ui);

    char filename[50];
    snprintf(filename, 50, "%s/resized.png", OUTPUT_FOLDER);
    SDL_Surface *gs = load_image(filename);
    surface_to_grayscale(gs);
    snprintf(filename, 50, "%s/grayscaled.png", OUTPUT_FOLDER);
    IMG_SavePNG(gs, filename);

    GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file(filename, NULL);
    GdkPixbuf *smol_pixbuf
        = gdk_pixbuf_scale_simple(pixbuf, 600, 600, GDK_INTERP_BILINEAR);

    GtkImage *image
        = GTK_IMAGE(gtk_builder_get_object(ui->builder, "grayscale_image"));
    gtk_image_set_from_pixbuf(image, smol_pixbuf);

    SDL_FreeSurface(gs);
}

static void on_ui_unload_grayscale(ui_t *ui)
{
    GtkImage *image
        = GTK_IMAGE(gtk_builder_get_object(ui->builder, "grayscale_image"));
    gtk_image_clear(image);
}

static void ui_load_grayscale(ui_t *ui)
{

    GtkContainer *w = ui_get_frame_by_id(ui, "gray");
    ui_frame_t res = (ui_frame_t) {.frame = w,
        .on_load = on_ui_load_grayscale,
        .on_unload = on_ui_unload_grayscale};
    ui->frames[GRAYSCALE_FRAME] = res;

    GtkButton *to_binary
        = GTK_BUTTON(gtk_builder_get_object(ui->builder, "go_binary"));
    g_signal_connect(to_binary, "clicked", G_CALLBACK(on_next_clicked), ui);

    GtkButton *restart
        = GTK_BUTTON(gtk_builder_get_object(ui->builder, "restart_gray"));
    g_signal_connect(restart, "clicked", G_CALLBACK(on_ui_restart), ui);
}

// binary frame
static void on_ui_load_binary(ui_t *ui)
{
    UNUSED(ui);

    char filename[50];
    snprintf(filename, 50, "%s/grayscaled.png", OUTPUT_FOLDER);
    SDL_Surface *temp_surface = load_image(filename);
    SDL_Surface *bs = full_binary(temp_surface);
    snprintf(filename, 50, "%s/binarized.png", OUTPUT_FOLDER);
    IMG_SavePNG(bs, filename);

    GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file(filename, NULL);
    GdkPixbuf *smol_pixbuf
        = gdk_pixbuf_scale_simple(pixbuf, 600, 600, GDK_INTERP_BILINEAR);

    GtkImage *image
        = GTK_IMAGE(gtk_builder_get_object(ui->builder, "binarized_image"));
    gtk_image_set_from_pixbuf(image, smol_pixbuf);

    SDL_FreeSurface(temp_surface);
    SDL_FreeSurface(bs);
}

static void on_ui_unload_binary(ui_t *ui)
{
    GtkImage *image
        = GTK_IMAGE(gtk_builder_get_object(ui->builder, "binarized_image"));
    gtk_image_clear(image);
}

static void ui_load_binary(ui_t *ui)
{

    GtkContainer *w = ui_get_frame_by_id(ui, "binary");
    ui_frame_t res = (ui_frame_t) {.frame = w,
        .on_load = on_ui_load_binary,
        .on_unload = on_ui_unload_binary};
    ui->frames[BINARY_FRAME] = res;

    GtkButton *go_split
        = GTK_BUTTON(gtk_builder_get_object(ui->builder, "go_split"));
    g_signal_connect(go_split, "clicked", G_CALLBACK(on_next_clicked), ui);

    GtkButton *restart
        = GTK_BUTTON(gtk_builder_get_object(ui->builder, "restart_binary"));
    g_signal_connect(restart, "clicked", G_CALLBACK(on_ui_restart), ui);
}

// split frame
static void on_ui_load_split(ui_t *ui)
{
    UNUSED(ui);

    GtkLabel *label
        = GTK_LABEL(gtk_builder_get_object(ui->builder, "split_result"));
    GtkGrid *grid
        = GTK_GRID(gtk_builder_get_object(ui->builder, "split_grid"));

    char bin[50], gscaled[50], split_path[50];
    snprintf(bin, 50, "%s/binarized.png", OUTPUT_FOLDER);
    snprintf(gscaled, 50, "%s/grayscaled.png", OUTPUT_FOLDER);
    snprintf(split_path, 50, "%s/split/", OUTPUT_FOLDER);

    mkdir(split_path, 0777);

    if (!sudoku_split(bin, gscaled, split_path, ui->process.grid_size))
    {
        gtk_widget_show(GTK_WIDGET(grid));

        for (int i = 0; i < ui->process.grid_size; i++)
        {
            for (int j = 0; j < ui->process.grid_size; j++)
            {
                char split_no[70];
                snprintf(split_no, 70, "%s%i.png", split_path,
                    i + j * ui->process.grid_size);

                GtkWidget *image = gtk_image_new_from_file(split_no);

                gtk_grid_attach(grid, image, i, j, 1, 1);

                gtk_widget_show(image);
            }
        }

        gtk_label_set_text(label, "The image has been split.");
    }
    else
    {
        gtk_widget_hide(GTK_WIDGET(grid));
        gtk_label_set_text(label, "Splitting has failed.");
    }
}

static void on_ui_unload_split(ui_t *ui)
{
    UNUSED(ui);

    GtkContainer *container
        = GTK_CONTAINER(gtk_builder_get_object(ui->builder, "split_grid"));
    GList *children = gtk_container_get_children(container);

    guint length = g_list_length(children);
    for (size_t i = 0; i < length; i++)
        gtk_widget_destroy(g_list_nth_data(children, i));
}

static void ui_load_split(ui_t *ui)
{

    GtkContainer *w = ui_get_frame_by_id(ui, "split");
    ui_frame_t res = (ui_frame_t) {.frame = w,
        .on_load = on_ui_load_split,
        .on_unload = on_ui_unload_split};
    ui->frames[SPLIT_FRAME] = res;

    GtkButton *to_results
        = GTK_BUTTON(gtk_builder_get_object(ui->builder, "go_results"));
    g_signal_connect(to_results, "clicked", G_CALLBACK(on_next_clicked), ui);

    GtkButton *restart
        = GTK_BUTTON(gtk_builder_get_object(ui->builder, "restart_split"));
    g_signal_connect(restart, "clicked", G_CALLBACK(on_ui_restart), ui);
}

// results frame
static void on_ui_load_results(ui_t *ui)
{
    UNUSED(ui);

    GtkGrid *grid
        = GTK_GRID(gtk_builder_get_object(ui->builder, "results_grid"));
    GtkImage *image
        = GTK_IMAGE(gtk_builder_get_object(ui->builder, "results_image"));

    int *res = convert_multi(SPLIT_FOLDER, ui->process.net, 0.4);

    for (int i = 0; i < ui->process.grid_size; i++)
    {
        for (int j = 0; j < ui->process.grid_size; j++)
        {
            int n = res[ui->process.grid_size * j + i];
            GtkWidget *entry = gtk_entry_new();

            gtk_grid_attach(grid, entry, i, j, 1, 1);

            char entry_text[20];
            if (n != 0)
                if (n >= 10)
                    snprintf(entry_text, 20, "%c", n - 10 + 'A');
                else
                    snprintf(entry_text, 20, "%i", n);
            else
                snprintf(entry_text, 20, " ");

            char entry_name[20];
            snprintf(entry_name, 20, "entry%i", n);

            GtkStyleContext *context
                = gtk_widget_get_style_context(GTK_WIDGET(entry));
            if (ui->process.grid_size == 9)
                gtk_style_context_add_class(context, "results_entry9");
            else
                gtk_style_context_add_class(context, "results_entry16");

            gtk_widget_set_name(entry, entry_name);
            gtk_entry_set_width_chars(GTK_ENTRY(entry), 2);
            gtk_widget_show(entry);
            gtk_entry_set_text(GTK_ENTRY(entry), entry_text);
        }
    }

    char filename[50];
    snprintf(filename, 50, "%s/rotated.png", OUTPUT_FOLDER);

    GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file(filename, NULL);
    GdkPixbuf *smol_pixbuf
        = gdk_pixbuf_scale_simple(pixbuf, 600, 600, GDK_INTERP_BILINEAR);

    SDL_Surface *temp_surface = load_image(filename);
    SDL_Surface *bs = full_binary(temp_surface);
    snprintf(filename, 50, "%s/binarized.png", OUTPUT_FOLDER);
    IMG_SavePNG(bs, filename);

    gtk_image_set_from_pixbuf(image, smol_pixbuf);

    free(res);
}

static void on_ui_unload_results(ui_t *ui)
{
    UNUSED(ui);

    GtkContainer *container
        = GTK_CONTAINER(gtk_builder_get_object(ui->builder, "results_grid"));
    GList *children = gtk_container_get_children(container);

    guint length = g_list_length(children);
    for (size_t i = 0; i < length; i++)
        gtk_container_remove(container, g_list_nth_data(children, i));
}

static void ui_load_results(ui_t *ui)
{
    GtkContainer *w = ui_get_frame_by_id(ui, "results");
    ui_frame_t res = (ui_frame_t) {.frame = w,
        .on_load = on_ui_load_results,
        .on_unload = on_ui_unload_results};
    ui->frames[RESULTS_FRAME] = res;

    GtkButton *restart
        = GTK_BUTTON(gtk_builder_get_object(ui->builder, "restart_results"));
    g_signal_connect(restart, "clicked", G_CALLBACK(on_ui_restart), ui);

    GtkButton *to_complete
        = GTK_BUTTON(gtk_builder_get_object(ui->builder, "go_complete"));
    g_signal_connect(to_complete, "clicked", G_CALLBACK(on_validate), ui);
}

// complete frame
static void on_ui_load_complete(ui_t *ui)
{
    UNUSED(ui);

    GtkImage *image
        = GTK_IMAGE(gtk_builder_get_object(ui->builder, "complete_image"));

    char filename[50];
    snprintf(filename, 50, "%s/completed.png", OUTPUT_FOLDER);
    gtk_image_set_from_file(image, (const gchar *)filename);
}

static void on_ui_unload_complete(ui_t *ui)
{
    GtkImage *image
        = GTK_IMAGE(gtk_builder_get_object(ui->builder, "complete_image"));
    gtk_image_clear(image);
}

static void ui_load_complete(ui_t *ui)
{

    GtkContainer *w = ui_get_frame_by_id(ui, "complete");
    ui_frame_t res = (ui_frame_t) {.frame = w,
        .on_load = on_ui_load_complete,
        .on_unload = on_ui_unload_complete};
    ui->frames[COMPLETE_FRAME] = res;

    GtkButton *restart
        = GTK_BUTTON(gtk_builder_get_object(ui->builder, "restart_complete"));
    g_signal_connect(restart, "clicked", G_CALLBACK(on_ui_restart), ui);

    GtkButton *quit = GTK_BUTTON(gtk_builder_get_object(ui->builder, "quit"));
    g_signal_connect(quit, "clicked", G_CALLBACK(on_quit_b), ui);
}

void open_ui()
{
    gtk_init(0, NULL);

    GtkBuilder *builder = gtk_builder_new_from_file("resources/ui/ui.glade");
    ui_t *ui = build_ui(builder);
    struct ui_user_data **user_data_list = NULL;
    size_t user_data_size = 0;

    ui_load_upload(ui);

    ui_load_resize(ui, &user_data_list, &user_data_size);

    ui_load_rotate(ui);

    ui_load_grayscale(ui);

    ui_load_binary(ui);

    ui_load_split(ui);

    ui_load_results(ui);

    ui_load_complete(ui);

    ui_go_to(ui, UPLOAD_FRAME);

    g_signal_connect(ui->window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    g_signal_connect(ui->window, "destroy", G_CALLBACK(on_quit_w), ui);
    gtk_window_set_title(ui->window, "Lezcollitade");

    gtk_builder_connect_signals(builder, NULL);

    GtkCssProvider *cssProvider = gtk_css_provider_new();

    gtk_css_provider_load_from_path(cssProvider, "resources/ui/gtk.css", NULL);

    GdkScreen *screen = gdk_screen_get_default();
    gtk_style_context_add_provider_for_screen(screen,
        GTK_STYLE_PROVIDER(cssProvider), GTK_STYLE_PROVIDER_PRIORITY_USER);

    gtk_widget_show(GTK_WIDGET(ui->window));

    gtk_main();
    for (size_t i = 0; i < user_data_size; i++)
    {
        free(user_data_list[i]);
    }
    free(user_data_list);
    ui_free(ui);
}
