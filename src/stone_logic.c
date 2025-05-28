//
// Created by stefan on 8/05/25.
//

#include <dbg.h>
#include "stone_logic.h"
#include "groups.h"

extern u_char board_array[];
int captured_black = 0;
int captured_white = 0;

static StoneType invert_stone_type(StoneType stone_type) {
    if (stone_type == EMPTY) {
        return EMPTY;
    }
    if (stone_type == BLACK_STONE) {
        return WHITE_STONE;
    }
    if (stone_type == WHITE_STONE) {
        return BLACK_STONE;
    }
    log_err("Trying to invert non-existing StoneType");
    return EMPTY;
}

static void get_board_indices_from_grid(GtkWidget* board_grid, gdouble x, gdouble y, u_char *x_index, u_char *y_index) {
    /* Get the x index of the played move on the board */
    for (int i = 0; i < gtk_widget_get_width(board_grid); i++) {
        if (i * STONE_SIZE + (i - 1) * STONE_SPACING > x) {
            *x_index = i - 1;
            break;
        }
    }

    /* Get the y index of the played move on the board */
    for (int i = 0; i < gtk_widget_get_height(board_grid); i++) {
        if (i * STONE_SIZE + (i - 1) * STONE_SPACING > y) {
            *y_index = i - 1;
            break;
        }
    }
}

static void get_stone_group(Group* group, u_char x_index, u_char y_index) {
    int board_array_index = x_index + y_index * (int) sqrt(BOARD_SIZE);

    /* Checking if a stone is on the edge of the board */
    bool right_edge_stone = x_index % ((u_char) sqrt(BOARD_SIZE) - 1) == 0 && x_index != 0;
    bool left_edge_stone = x_index % (u_char) sqrt(BOARD_SIZE) == 0;
    bool bottom_edge_stone = y_index % ((u_char) sqrt(BOARD_SIZE) - 1) == 0 && y_index != 0;
    bool top_edge_stone = y_index % (u_char) sqrt(BOARD_SIZE) == 0;

    StoneType placed_stone_color = board_array[x_index + y_index * (int) sqrt(BOARD_SIZE)];

    if (placed_stone_color == EMPTY) {
        return;
    }

    BoardCoord *board_coord = create_board_coord(x_index, y_index);
    insert_group(group, board_coord);

    /* TODO: rewrite, duplicate code */
    if (board_array[board_array_index + 1] == placed_stone_color && !right_edge_stone) {
        if (!group_contains_indices(group, x_index + 1, y_index)) {
            get_stone_group(group, x_index + 1, y_index);
        }
    }
    if (board_array[board_array_index - 1] == placed_stone_color && !left_edge_stone) {
        if (!group_contains_indices(group, x_index - 1, y_index)) {
            get_stone_group(group, x_index - 1, y_index);
        }
    }
    if (board_array[board_array_index + (u_char) sqrt(BOARD_SIZE)] == placed_stone_color && !bottom_edge_stone) {
        if (!group_contains_indices(group, x_index, y_index + 1)) {
            get_stone_group(group, x_index, y_index + 1);
        }
    }
    if (board_array[board_array_index - (u_char) sqrt(BOARD_SIZE)] == placed_stone_color && !top_edge_stone) {
        if (!group_contains_indices(group, x_index, y_index - 1)) {
            get_stone_group(group, x_index, y_index - 1);
        }
    }
}

static u_short get_group_liberties(const Group* group) {
    u_short liberties = 0;
    Group counted_liberties;
    init_group(&counted_liberties, AVERAGE_GROUP_SIZE);
    for (size_t i = 0; i < group->amount; i++) {
        u_char x_index = group->group[i]->x_index;
        u_char y_index = group->group[i]->y_index;

        int board_array_index = x_index + y_index * (int) sqrt(BOARD_SIZE);

        /* Checking if a stone is on the edge of the board and the adjacent intersections are empty */
        /* right edge stone */
        if ((x_index % ((u_char) sqrt(BOARD_SIZE) - 1) != 0 || x_index == 0) && board_array[board_array_index + 1] == EMPTY) {
            if (!group_contains_indices(&counted_liberties, x_index + 1, y_index)) {
                BoardCoord* board_coord = create_board_coord(x_index + 1, y_index);
                insert_group(&counted_liberties, board_coord);
                liberties++;
            }
        }
        /* left edge stone */
        if (x_index % (u_char) sqrt(BOARD_SIZE) != 0 && board_array[board_array_index - 1] == EMPTY) {
            if (!group_contains_indices(&counted_liberties, x_index - 1, y_index)) {
                BoardCoord* board_coord = create_board_coord(x_index - 1, y_index);
                insert_group(&counted_liberties, board_coord);
                liberties++;
            }
        }
        /* bottom edge stone */
        if ((y_index % ((u_char) sqrt(BOARD_SIZE) - 1) != 0 || y_index == 0) && board_array[board_array_index + (int) sqrt(BOARD_SIZE)] == EMPTY) {
            if (!group_contains_indices(&counted_liberties, x_index, y_index + 1)) {
                BoardCoord* board_coord = create_board_coord(x_index, y_index + 1);
                insert_group(&counted_liberties, board_coord);
                liberties++;
            }
        }
        /* top edge stone */
        if (y_index % (u_char) sqrt(BOARD_SIZE) != 0 && board_array[board_array_index - (u_char) sqrt(BOARD_SIZE)] == EMPTY) {
            if (!group_contains_indices(&counted_liberties, x_index, y_index - 1)) {
                BoardCoord* board_coord = create_board_coord(x_index, y_index - 1);
                insert_group(&counted_liberties, board_coord);
                liberties++;
            }
        }
    }

    return liberties;
}

/**
 * Function to capture a group
 * @param board_grid    pointer to the board grid widget
 * @param group         pointer to the group to be captured
 * @param stone_type    the StoneType of the group of stones
 */
static void capture_group(GtkWidget *board_grid, const Group* group, StoneType stone_type) {
    if (stone_type == EMPTY) {
        log_err("stone_color is EMPTY_STONE. Can't capture empty group.");
        return;
    }
    for (size_t i = 0; i < group->amount; i++) {
        BoardCoord* board_coord = group->group[i];
        ((u_char*)board_array)[board_coord->x_index + board_coord->y_index * (int) sqrt(BOARD_SIZE)] = EMPTY;
        GtkWidget *image = gtk_grid_get_child_at(GTK_GRID(board_grid), board_coord->x_index, board_coord->y_index);
        gtk_grid_remove(GTK_GRID(board_grid), image);
        GtkWidget *empty = gtk_image_new_from_file(EMPTY_PATH);

        gtk_image_set_pixel_size(GTK_IMAGE(empty), STONE_SIZE);
        gtk_grid_attach(GTK_GRID(board_grid), empty, board_coord->x_index, board_coord->y_index, 1, 1);
    }
    if (stone_type == BLACK_STONE) {
        captured_black += (int) group->amount;
    } else if (stone_type == WHITE_STONE) {
        captured_white += (int) group->amount;
    }
}

/**
 * Takes a reference to an array of min. 4 Group pointers and will assign the groups captured by a move
 * to the array in the following order: top, right, bottom, left.
 * THE CALLER OF THE FUNCTION MUST FREE THE RETURNED GROUPS.
 * @param groups        an array of Group pointers (min. size = 4)
 * @param x_index       the x index of the played stone
 * @param y_index       the y index of the played stone
 * @param stone_type    the StoneType of the group of stones
 */
static void get_captured_groups(Group *groups[], u_char x_index, u_char y_index, StoneType stone_type) {
    int board_array_index = x_index + y_index * (int) sqrt(BOARD_SIZE);

    for (u_char i = 0; i < 4; i++) {
        Group *opponent_group = malloc(sizeof(Group));
        init_group(opponent_group, AVERAGE_GROUP_SIZE);
        bool group_found = false;

        switch (i) {
            case 0:
                if (((u_char*)board_array)[board_array_index - (u_char) sqrt(BOARD_SIZE)] == stone_type) {
                    group_found = true;
                    get_stone_group(opponent_group, x_index, y_index - 1);
                }
                break;
            case 1:
                if (((u_char*)board_array)[board_array_index + 1] == stone_type) {
                    group_found = true;
                    get_stone_group(opponent_group, x_index + 1, y_index);
                }
                break;
            case 2:
                if (((u_char*)board_array)[board_array_index + (u_char) sqrt(BOARD_SIZE)] == stone_type) {
                    group_found = true;
                    get_stone_group(opponent_group, x_index, y_index + 1);
                }
                break;
            case 3:
                if (((u_char*)board_array)[board_array_index - 1] == stone_type) {
                    group_found = true;
                    get_stone_group(opponent_group, x_index - 1, y_index);
                }
                break;
            default:
                log_err("Impossible default case");
        }

        if (group_found && get_group_liberties(opponent_group) == 0) {
            groups[i] = opponent_group;
        } else {
            groups[i] = NULL;
            free_group(opponent_group);
            free(opponent_group);
        }
    }

    /* TODO: needs a rewrite, duplicate code */
    // if (((u_char*)board_array)[board_array_index + 1] == stone_type) {
    //     Group *opponent_group = malloc(sizeof(Group));
    //     init_group(opponent_group, AVERAGE_GROUP_SIZE);
    //     get_stone_group(opponent_group, x_index + 1, y_index);
    //     if (get_group_liberties(opponent_group) == 0) {
    //         groups[1] = opponent_group;
    //     } else {
    //         groups[1] = NULL;
    //     }
    // }
    // if (((u_char*)board_array)[board_array_index - 1] == stone_type) {
    //     Group opponent_group;
    //     init_group(&opponent_group, AVERAGE_GROUP_SIZE);
    //     get_stone_group(&opponent_group, x_index - 1, y_index);
    //     if (get_group_liberties(&opponent_group) == 0) {
    //         groups[3] = &opponent_group;
    //     } else {
    //         groups[3] = NULL;
    //     }
    // }
    // if (((u_char*)board_array)[board_array_index + (u_char) sqrt(BOARD_SIZE)] == stone_type) {
    //     Group opponent_group;
    //     init_group(&opponent_group, AVERAGE_GROUP_SIZE);
    //     get_stone_group(&opponent_group, x_index, y_index + 1);
    //     if (get_group_liberties(&opponent_group) == 0) {
    //         groups[2] = &opponent_group;
    //     } else {
    //         groups[2] = NULL;
    //     }
    // }
    // if (((u_char*)board_array)[board_array_index - (u_char) sqrt(BOARD_SIZE)] == stone_type) {
    //     Group opponent_group;
    //     init_group(&opponent_group, AVERAGE_GROUP_SIZE);
    //     get_stone_group(&opponent_group, x_index, y_index - 1);
    //     if (get_group_liberties(&opponent_group) == 0) {
    //         groups[0] = &opponent_group;
    //     } else {
    //         groups[0] = NULL;
    //     }
    // }
}

bool check_valid_play(u_char x_index, u_char y_index, StoneType stone_type) {
    Group group;
    init_group(&group, AVERAGE_GROUP_SIZE);

    /* 'Image' a stone being on the x and y location and add it to the group */
    int board_array_index = x_index + y_index * (int) sqrt(BOARD_SIZE);
    StoneType current_stone = board_array[board_array_index];
    board_array[board_array_index] = stone_type;

    get_stone_group(&group, x_index, y_index);

    g_print("%d\n", get_group_liberties(&group));
    if (get_group_liberties(&group) == 0) {
        free_group(&group);
        board_array[board_array_index] = current_stone;
        return false;
    }
    free_group(&group);
    board_array[board_array_index] = current_stone;

    return true;
}

void play_stone(GtkGestureClick *gesture, gint n_press, gdouble x, gdouble y, gpointer board_array) {
    GtkWidget *board_grid = gtk_event_controller_get_widget(GTK_EVENT_CONTROLLER(gesture));
    static int current_color = BLACK_STONE;
    u_char x_index;
    u_char y_index;

    // Use gtk_gesture_single_get_current_button() to detect right clicks
    // g_print("Button number: %d\n", gtk_gesture_single_get_current_button(GTK_GESTURE_SINGLE(gesture)));

    /* Get x and y index on the board */
    get_board_indices_from_grid(board_grid, x, y, &x_index, &y_index);

    int board_array_index = x_index + y_index * (int) sqrt(BOARD_SIZE);

    /* Check if the point is empty and play the stone */
    if (((u_char*)board_array)[board_array_index] == EMPTY && check_valid_play(x_index, y_index, current_color)) {
        GtkWidget *placing_stone;
        ((u_char*)board_array)[board_array_index] = current_color;

        GtkWidget *image = gtk_grid_get_child_at(GTK_GRID(board_grid), x_index, y_index);
        gtk_grid_remove(GTK_GRID(board_grid), image);
        if (current_color == BLACK_STONE) {
            placing_stone = gtk_image_new_from_file(BLACK_STONE_PATH);
        } else if (current_color == WHITE_STONE) {
            placing_stone = gtk_image_new_from_file(WHITE_STONE_PATH);
        } else {
            // sentinel("Incorrect stone placement")
            return;
        }
        current_color = invert_stone_type(current_color);

        gtk_image_set_pixel_size(GTK_IMAGE(placing_stone), STONE_SIZE);
        gtk_grid_attach(GTK_GRID(board_grid), placing_stone, x_index, y_index, 1, 1);


        /* Capture groups that don't have any liberties left.
         * Here, current_color is already updated, so this refers to the opponent color.
         */
        Group *groups[4];
        get_captured_groups(groups, x_index, y_index, current_color);
        for (u_char i = 0; i < 4; i++) {
            if (groups[i] == NULL) continue;
            capture_group(board_grid, groups[i], current_color);
            /* First free the 'vector' that contains the BoardCoords,
             * then free the actual group.
             */
            free_group(groups[i]);
            free(groups[i]);
        }
    }
}