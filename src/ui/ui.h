#ifndef UI_UI_H
#define UI_UI_H

/**
 * \brief Changes stack's page if a file was selected
 * \fn void on_start(GtkWidget *w, gpointer data)
 * \param w The widget calling the function
 * \param data The page to switch to
 */
void on_start(GtkWidget *w, gpointer data);

/**
 * \brief Changes the stack's displayed page
 * \fn void go_to(GtkWidget *w, gpointer data)
 * \param w The widget calling the function
 * \param data The page to switch to
 */
void go_to(GtkWidget *w, gpointer data);

/**
 * \brief Sets up the binary page
 * \fn void load_binary()
 */
void load_binary();

/**
 * \brief Sets up the grayscale page
 * \fn void load_grayscale()
 */
void load_grayscale();

/**
 * \brief Sets up the rotate page
 * \fn void load_rotate()
 */
void load_rotate();

/**
 * \brief Updates rotation when the scrollbar is moved
 * \fn void on_scrollbar_value_changed(GtkRange *r)
 * \param r The scrollbar moved
 */
void on_scrollbar_value_changed(GtkRange *r);

/**
 * \brief Copies the chosen file to the ocr directory
 * \fn void on_import_file_set(GtkFileChooserButton *f)
 * \param f The button activating it
 */
void on_import_file_set(GtkFileChooserButton *f);

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

#endif
