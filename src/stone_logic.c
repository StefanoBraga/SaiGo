//
// Created by stefan on 8/05/25.
//

#include "stone_logic.h"

enum StoneType {
    EMPTY = 0,
    BLACK_STONE = 1,
    WHITE_STONE = 2,
};

int current_color = BLACK_STONE;

// static int get_liberties(int* board_array, unsigned int x_index, unsigned int y_index) {
//     int liberties = 0;
//
//     StoneType stone_color = board_array[x_index + y_index * (int) sqrt(BOARD_SIZE)];
//
//     return liberties;
// }

void play_stone(GtkGestureClick *gesture, gint n_press, gdouble x, gdouble y, gpointer board_array) {
    //g_print("x=%f, y=%f, n_press=%d\n", x, y, n_press);

    GtkWidget *board_grid = gtk_event_controller_get_widget(GTK_EVENT_CONTROLLER(gesture));

    int x_index = 0;
    for (int i = 0; i < gtk_widget_get_width(board_grid); i++) {
        if (i * STONE_SIZE + (i - 1) * STONE_SPACING > x) {
            x_index = i - 1;
            break;
        }
    }

    int y_index = 0;
    for (int i = 0; i < gtk_widget_get_height(board_grid); i++) {
        if (i * STONE_SIZE + (i - 1) * STONE_SPACING > y) {
            y_index = i - 1;
            break;
        }
    }

    int board_array_index = x_index + y_index * (int) sqrt(BOARD_SIZE);
    //g_print("Board index: %d\nBoard value:%d\n", board_array_index, ((int*)board_array)[board_array_index]);

    if (((int*)board_array)[board_array_index] == EMPTY) {
        GtkWidget *placing_stone;
        ((int*)board_array)[board_array_index] = current_color;

        GtkWidget *image = gtk_grid_get_child_at(GTK_GRID(board_grid), x_index, y_index);
        gtk_grid_remove(GTK_GRID(board_grid), image);
        if (current_color == BLACK_STONE) {
            placing_stone = gtk_image_new_from_file(BLACK_STONE_PATH);
            current_color = WHITE_STONE;
        } else if (current_color == WHITE_STONE) {
            placing_stone = gtk_image_new_from_file(WHITE_STONE_PATH);
            current_color = BLACK_STONE;
        } else {
            // sentinel("Incorrect stone placement")
            return;
        }

        gtk_image_set_pixel_size(GTK_IMAGE(placing_stone), STONE_SIZE);

        gtk_grid_attach(GTK_GRID(board_grid), placing_stone, x_index, y_index, 1, 1);
    }
}