//
// Created by stefan on 8/05/25.
//

#ifndef STONE_LOGIC_H
#define STONE_LOGIC_H

#include <gtk/gtk.h>
#include <consts.h>

typedef enum StoneType {
    EMPTY = 0,
    BLACK_STONE = 1,
    WHITE_STONE = 2,
} StoneType;

extern bool check_valid_play(u_char x_index, u_char y_index, StoneType stone_type);
extern bool check_liberties(u_char x_index, u_char y_index, StoneType stone_type);
extern void place_stone(u_char x_index, u_char y_index, StoneType stone_type);
extern void play_stone(GtkGestureClick *gesture, gint n_press, gdouble x, gdouble y, gpointer user_data);


#endif //STONE_LOGIC_H
