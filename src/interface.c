//
// Created by stefanv on 5/29/25.
//
#include <gtk/gtk.h>

#include "consts.h"
#include "types.h"       /* Needed by interface.h. */
#include "stone_logic.h" /* Needed by interface.h. */
#include "interface.h"

GtkWidget*
init_window(GtkApplication* app)
{
    GtkWidget *window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "SaiGo");
    gtk_window_set_default_size(GTK_WINDOW(window), 750, 750);
    gtk_window_set_resizable(GTK_WINDOW(window), false);

    return window;
}

void
init_setting_fields(GtkBuilder *builder, struct SettingFields *setting_fields)
{
    setting_fields->settings_window = gtk_builder_get_object(builder, "settings_window");
    setting_fields->black_name_entry = gtk_builder_get_object(builder, "black_name_entry");
    setting_fields->white_name_entry = gtk_builder_get_object(builder, "white_name_entry");
    setting_fields->scoring_dropdown = gtk_builder_get_object(builder, "scoring_dropdown");
    setting_fields->komi_entry = gtk_builder_get_object(builder, "komi_entry");
}

void
init_board(GtkGrid *board_grid, GdkTexture *empty_texture)
{
    for (int i = 0; i < BOARD_SIZE; i++) {
        GtkWidget *empty =
                gtk_image_new_from_paintable(GDK_PAINTABLE(empty_texture));
        gtk_image_set_pixel_size(GTK_IMAGE(empty), STONE_SIZE);

        // The third and fourth argument are the grid indexes and the fifth and
        // sixth is stretching
        gtk_grid_attach(board_grid, empty, i % (int) sqrt(BOARD_SIZE),
                        i / (int) sqrt(BOARD_SIZE), 1, 1);
    }
}

void
add_stone_to_board(GtkWidget* board_grid, u_char x_index, u_char y_index, StoneType stone_type)
{
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
