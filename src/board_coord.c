//
// Created by stefanv on 5/28/25.
//

#include <stdlib.h>
#include "board_coord.h"

BoardCoord *create_board_coord(u_char x_index, u_char y_index) {
    BoardCoord *board_coord = malloc(sizeof(BoardCoord));
    board_coord->x_index = x_index;
    board_coord->y_index = y_index;
    return board_coord;
}