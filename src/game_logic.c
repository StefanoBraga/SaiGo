#include <gtk/gtk.h>      /* Needed by game_logic.h and stone_logic.h */

#include "consts.h"
#include "types.h"        /* Needed by game_logic.h and stone_logic.h */
#include "stone_logic.h"
#include "game_logic.h"

extern StoneType current_color;

struct Settings settings;
u_char board_array[BOARD_SIZE] = {0};

