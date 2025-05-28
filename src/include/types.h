//
// Created by stefan on 8/05/25.
//

#ifndef TYPES_H
#define TYPES_H

typedef struct BoardCoord {
    u_char x_index;
    u_char y_index;
} BoardCoord;

inline BoardCoord *create_board_coord(u_char x_index, u_char y_index) {
    BoardCoord *board_coord = malloc(sizeof(BoardCoord));
    board_coord->x_index = x_index;
    board_coord->y_index = y_index;
    return board_coord;
}

#endif //TYPES_H
