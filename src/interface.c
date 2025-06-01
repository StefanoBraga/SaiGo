//
// Created by stefanv on 5/29/25.
//

#include "interface.h"
#include "consts.h"

GtkWidget* init_window(GtkApplication* app) {
    GtkWidget *window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "SaiGo");
    gtk_window_set_default_size(GTK_WINDOW(window), 750, 750);
    gtk_window_set_resizable(GTK_WINDOW(window), false);

    return window;
}

void add_stone_to_board(GtkWidget* board_grid, u_char x_index, u_char y_index, StoneType stone_type) {
    GtkWidget *image = gtk_grid_get_child_at(GTK_GRID(board_grid), x_index, y_index);
    gtk_grid_remove(GTK_GRID(board_grid), image);
    GtkWidget *placing_stone;
    if (stone_type == BLACK_STONE) {
        placing_stone = gtk_image_new_from_file(BLACK_STONE_PATH);
    } else if (stone_type == WHITE_STONE) {
        placing_stone = gtk_image_new_from_file(WHITE_STONE_PATH);
    } else {
        // sentinel("Incorrect stone placement")
        return;
    }
    gtk_image_set_pixel_size(GTK_IMAGE(placing_stone), STONE_SIZE);
    gtk_grid_attach(GTK_GRID(board_grid), placing_stone, x_index, y_index, 1, 1);
}