#include <stdlib.h>
#include <glib/gstdio.h>
#include <gtk/gtk.h>

#include "consts.h"
#include "dbg.h"
#include "interface.h"
#include "stone_logic.h"

u_char board_array[BOARD_SIZE] = {0};

static void
activate(GtkApplication *app, gpointer user_data)
{
    GObject *window, *board_grid, *pass_button;
    GFile *empty_file = g_file_new_for_path(EMPTY_PATH);
    GdkTexture *empty_texture;
    GtkBuilder *builder = gtk_builder_new();

    GtkGesture *click_gesture = gtk_gesture_click_new();
    gtk_gesture_single_set_button(GTK_GESTURE_SINGLE(click_gesture), 0);

    /* Load our UI description */
    gtk_builder_add_from_file(builder, "../src/builder.ui", NULL);

    window = gtk_builder_get_object(builder, "window");
    board_grid = gtk_builder_get_object(builder, "board_grid");
    pass_button = gtk_builder_get_object(builder, "pass_button");

    empty_texture = gdk_texture_new_from_file(empty_file, NULL);
    if (empty_texture == NULL) {
        g_print("Error loading empty texture\n");
    }

    gtk_window_set_application(GTK_WINDOW(window), app);

    /* Set up the board. */
    for (int i = 0; i < BOARD_SIZE; i++) {
        GtkWidget *empty =
                gtk_image_new_from_paintable(GDK_PAINTABLE(empty_texture));
        gtk_image_set_pixel_size(GTK_IMAGE(empty), STONE_SIZE);

        // The third and fourth argument are the grid indexes and the fifth and
        // sixth is stretching
        gtk_grid_attach(GTK_GRID(board_grid), empty, i % (int) sqrt(BOARD_SIZE),
                        i / (int) sqrt(BOARD_SIZE), 1, 1);
    }

    /* Connecting the signals */
    gtk_widget_add_controller(GTK_WIDGET(board_grid), GTK_EVENT_CONTROLLER(click_gesture));
    g_signal_connect(click_gesture, "pressed", G_CALLBACK(play_stone), NULL);
    g_signal_connect(pass_button, "clicked", G_CALLBACK(pass), window);

    gtk_widget_set_visible(GTK_WIDGET(window), TRUE);

    g_object_unref(empty_file);
    g_object_unref(empty_texture);
    g_object_unref(builder);
}

int
main(int argc, char **argv)
{
    GtkApplication *app;
    int status;

    app = gtk_application_new("com.github.StefanoBraga.SaiGo",
                              0);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    status = g_application_run(G_APPLICATION(app), argc, argv);

    g_object_unref(app);

    return status;
}