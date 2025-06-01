//
// Created by stefanv on 5/29/25.
//

#ifndef INTERFACE_H
#define INTERFACE_H

#include <gtk/gtk.h>
#include "stone_logic.h"

extern GtkWidget* init_window(GtkApplication* app);
extern void add_stone_to_board(GtkWidget* board_grid, u_char x_index, u_char y_index, StoneType stone_type);

#endif //INTERFACE_H
