#include <stdlib.h>
#include <consts.h>
#include "stone_logic.h"
#include <dbg.h>
#include <glib/gstdio.h>
#include <gtk/gtk.h>

u_char board_array[BOARD_SIZE] = {0};

static void activate(GtkApplication *app, gpointer user_data) {
    GtkWidget *window;

    window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "SaiGo");
    gtk_window_set_default_size(GTK_WINDOW(window), 750, 750);
    gtk_window_set_resizable(GTK_WINDOW(window), false);

    GFile *black_stone_file = g_file_new_for_path(BLACK_STONE_PATH);
    GFile *empty_file = g_file_new_for_path(EMPTY_PATH);

    GdkTexture *black_stone_texture =
            gdk_texture_new_from_file(black_stone_file, NULL);
    GdkTexture *empty_texture = gdk_texture_new_from_file(empty_file, NULL);

    if (black_stone_texture == NULL) {
        g_print("Error loading black stone texture\n");
    }
    if (empty_texture == NULL) {
        g_print("Error loading empty texture\n");
    }

    GtkWidget *wood_image = gtk_image_new_from_file(WOOD_PATH);
    gtk_image_set_pixel_size(GTK_IMAGE(wood_image), 750);
    GtkWidget *board_image = gtk_image_new_from_file(BOARD_LINES_PATH);
    gtk_image_set_pixel_size(GTK_IMAGE(board_image), 750);

    GtkWidget *main_grid = gtk_grid_new();
    GtkWidget *board_grid = gtk_grid_new();

    gtk_grid_set_column_spacing(GTK_GRID(board_grid), STONE_SPACING);
    gtk_grid_set_row_spacing(GTK_GRID(board_grid), STONE_SPACING);

    GtkGesture *click_gesture = gtk_gesture_click_new();
    gtk_gesture_single_set_button(GTK_GESTURE_SINGLE(click_gesture), 0);

    for (int i = 0; i < BOARD_SIZE; i++) {
        GtkWidget *empty =
                gtk_image_new_from_paintable(GDK_PAINTABLE(empty_texture));
        gtk_image_set_pixel_size(GTK_IMAGE(empty), STONE_SIZE);

        // The third and fourth argument are the grid indexes and the fifth and
        // sixth is stretching
        gtk_grid_attach(GTK_GRID(board_grid), empty, i % (int) sqrt(BOARD_SIZE),
                        i / (int) sqrt(BOARD_SIZE), 1, 1);
    }

    GtkWidget *overlay = gtk_overlay_new();
    gtk_widget_set_halign(overlay, GTK_ALIGN_FILL);
    gtk_widget_set_valign(overlay, GTK_ALIGN_FILL);

    gtk_window_set_child(GTK_WINDOW(window), main_grid);

    gtk_overlay_set_child(GTK_OVERLAY(overlay), wood_image);
    gtk_overlay_add_overlay(GTK_OVERLAY(overlay), board_image);
    gtk_overlay_add_overlay(GTK_OVERLAY(overlay), board_grid);

    gtk_widget_set_margin_start(board_grid, BOARD_GRID_MARGIN);
    gtk_widget_set_margin_top(board_grid, BOARD_GRID_MARGIN);
    gtk_widget_set_margin_bottom(board_grid, BOARD_GRID_MARGIN);
    gtk_widget_set_margin_end(board_grid, BOARD_GRID_MARGIN);

    gtk_widget_add_controller(board_grid, GTK_EVENT_CONTROLLER(click_gesture));
    g_signal_connect(click_gesture, "pressed", G_CALLBACK(play_stone), board_array);

    gtk_grid_attach(GTK_GRID(main_grid), overlay, 0, 0, 1, 1);

    gtk_window_present(GTK_WINDOW(window));

    g_object_unref(black_stone_file);
    g_object_unref(empty_file);
    g_object_unref(black_stone_texture);
    g_object_unref(empty_texture);
}

int main(int argc, char **argv) {
    GtkApplication *app;
    int status;

    app = gtk_application_new("com.github.StefanoBraga.SaiGo",
                              G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    status = g_application_run(G_APPLICATION(app), argc, argv);

    g_object_unref(app);

    return status;
}