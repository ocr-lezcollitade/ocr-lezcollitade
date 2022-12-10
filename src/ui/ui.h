#ifndef UI_UI_H
#define UI_UI_H

#include "../network/network.h"
#define OUTPUT_FOLDER "/tmp/lezcollitade"
#define SPLIT_FOLDER "/tmp/lezcollitade/split/"

#define NET_MAP_9 "network-maps/sudoku/sudoku9.network"
#define NET_MAP_16 "network-maps/sudoku/sudoku16.network"

#define UPLOAD_FRAME 0
#define ROTATE_FRAME UPLOAD_FRAME + 1
#define RESIZE_FRAME ROTATE_FRAME + 1
#define GRAYSCALE_FRAME RESIZE_FRAME + 1
#define BINARY_FRAME GRAYSCALE_FRAME + 1
#define SPLIT_FRAME BINARY_FRAME + 1
#define RESULTS_FRAME SPLIT_FRAME + 1
#define COMPLETE_FRAME RESULTS_FRAME + 1

typedef struct ui_t ui_t;

typedef void (*on_ui_frame_load_cb_t)(ui_t *ui);

typedef struct
{
    GtkContainer *frame; /*! The pointer to the container */
    on_ui_frame_load_cb_t
        on_load; /*! The callback to run when frame is loaded */
    on_ui_frame_load_cb_t
        on_unload; /*! The callback to run when the frame is unloaded */
} ui_frame_t;

struct ui_user_data
{
    ui_t *ui;
    gpointer user_data;
};

struct process
{
    int grid_size;            /*! The dimension of the grid (i.e. 9 or 16) */
    int *completed_grid;      /*! The sudoku solved grid */
    int auto_rotate;          /*! Whether auto_rotate is on or not */
    GdkPixbuf *resize_pixbuf; /*! The pixel buffer for the resize step */
    network_t *net;           /*! The newtork */
};

struct ui_t
{
    GtkBuilder *builder;
    GtkWindow *window;  /*! The link to the window */
    GtkStack *stack;    /*! The link to the Stack Pane */
    ui_frame_t *frames; /*! The linked list to the frames in order. */
    size_t frame_index; /*! The index of the current frame */
    size_t frame_count; /*! The number of frames in the ui */
    struct process process;
};

/**
 *  \brief Frees the ui structure.
 */
void ui_free(ui_t *ui);

/**
 *  \brief  Goes to frame specified by index if existing.
 *  \fn     void ui_go_to(ui_t *ui, size_t index, gpointer user_data);
 *  \param  ui The pointer to the ui struct.
 *  \param index The index of the frame to switch to.
 */
void ui_go_to(ui_t *ui, size_t index);

/**
 *  \brief  Passes to the next frame in the ui.
 *  \fn     void ui_next_frame(ui_t *ui, gpointer user_data);
 *  \param  ui  The ui struct pointer.
 */
void ui_next_frame(ui_t *ui);

/**
 *  \brief  Restarts the ui and clears the current frame.
 *  \param ui   The ui.
 */
void ui_restart(ui_t *ui);

/**
 *  \brief  Returns the current ui frame.
 *  \fn     ui_frame_t ui_get_current_frame(ui_t *ui);
 *  \param  ui  The UI.
 *  \return The current UI frame.
 */
ui_frame_t ui_get_current_frame(ui_t *ui);

/**
 *  \brief  Builds the ui_t structure from the given builder.
 *  \fn     ui_t *build_ui(GtkBuilder *builder);
 *  \param builder The GtkBuilder.
 *  \return the built ui.
 */
ui_t *build_ui(GtkBuilder *builder);

/**
 * \brief Changes the stack's displayed page
 * \fn void go_to(GtkWidget *w, gpointer data)
 * \param w The widget calling the function
 * \param data The page to switch to
 */
void go_to(GtkButton *b, gpointer data);

/**
 * \brief Opens the UI
 * \fn void open_ui()
 */
void open_ui();

/**
 * \brief Quits the window
 * \fn void quit()
 */
void quit();

/**
 * \brief Blocks the start button
 * \fn void deselect()
 */
void deselect();

/**
 * \brief Applies an automatic rotation to the image before
 * calling load_grayscale
 * \fn void auto_rotation(GtkButton *b, gpointer user_data)
 * \param b The button activating it
 * \param user_data The struct ui
 */
void auto_rotation(GtkButton *b, gpointer user_data);

/**
 * \brief Fetches all the results in the entries and calls the drawerizer with
 * them \fn void on_validate(GtkButton *b, gpointer data) \param b The button
 * activating it \param user_data The struct ui
 */
void on_validate(GtkButton *b, gpointer data);

/**
 * \brief Quits the UI
 * \fn void on_quit(GtkButton *b, gpointer data)
 * \param b The button activating it
 * \param user_data The struct ui
 */
void on_quit(GtkButton *b, gpointer data);

/**
 * \brief Recursively removes a folder
 * \fn int recursive_rmdir(char *path)
 * \param path The path of the folder to remove
 */
int recursive_rmdir(char *path);

#endif
