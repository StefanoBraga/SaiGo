//
// Created by stefanv on 5/28/25.
//

#ifndef BOARD_COORD_H
#define BOARD_COORD_H

#include <sys/types.h>

typedef struct BoardCoord {
    u_char x_index;
    u_char y_index;
} BoardCoord;

BoardCoord *create_board_coord(u_char x_index, u_char y_index);

#endif //BOARD_COORD_H
