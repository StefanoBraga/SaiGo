//
// Created by stefan on 8/05/25.
//

#ifndef STONE_LOGIC_H
#define STONE_LOGIC_H

#include <gtk/gtk.h>
#include <consts.h>

typedef enum StoneType StoneType;

//static int get_liberties(int* board_array, unsigned int x_index, unsigned int y_index);

extern bool check_valid_play();
extern bool check_liberties(int* board_array, unsigned int x_index, unsigned int y_index);
extern void place_stone(GtkWidget* board_grid, unsigned int x_index, unsigned int y_index, StoneType stone_type);
extern void play_stone(GtkGestureClick *gesture, gint n_press, gdouble x, gdouble y, gpointer user_data);


#endif //STONE_LOGIC_H
