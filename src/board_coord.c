#include <stdlib.h>

#include "types.h"
#include "board_coord.h"

struct BoardCoord
*create_board_coord(u_char x_index, u_char y_index)
{
    struct BoardCoord *board_coord = malloc(sizeof(struct BoardCoord));
    board_coord->x_index = x_index;
    board_coord->y_index = y_index;
    return board_coord;
}
