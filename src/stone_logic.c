//
// Created by stefan on 8/05/25.
//

#include <dbg.h>
#include "stone_logic.h"
#include "groups.h"

extern u_char board_array[];
int captured_black = 0;
int captured_white = 0;

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

    BoardCoord *board_coord = malloc(sizeof(BoardCoord));
    board_coord->x_index = x_index;
    board_coord->y_index = y_index;
    insert_group(group, board_coord);

    if (board_array[board_array_index + 1] == placed_stone_color && !right_edge_stone) {
        bool already_in_group = false;
        for (size_t i = 0; i < group->amount; i++) {
            if (group->group[i]->x_index == x_index + 1 && group->group[i]->y_index == y_index) {
                already_in_group = true;
                break;
            }
        }
        if (!already_in_group) {
            get_stone_group(group, x_index + 1, y_index);
        }
    }
    if (board_array[board_array_index - 1] == placed_stone_color && !left_edge_stone) {
        bool already_in_group = false;
        for (size_t i = 0; i < group->amount; i++) {
            if (group->group[i]->x_index == x_index - 1 && group->group[i]->y_index == y_index) {
                already_in_group = true;
                break;
            }
        }
        if (!already_in_group) {
            get_stone_group(group, x_index - 1, y_index);
        }
    }
    if (board_array[board_array_index + (u_char) sqrt(BOARD_SIZE)] == placed_stone_color && !bottom_edge_stone) {
        bool already_in_group = false;
        for (size_t i = 0; i < group->amount; i++) {
            if (group->group[i]->x_index == x_index && group->group[i]->y_index == y_index + 1) {
                already_in_group = true;
                break;
            }
        }
        if (!already_in_group) {
            get_stone_group(group, x_index, y_index + 1);
        }
    }
    if (board_array[board_array_index - (u_char) sqrt(BOARD_SIZE)] == placed_stone_color && !top_edge_stone) {
        bool already_in_group = false;
        for (size_t i = 0; i < group->amount; i++) {
            if (group->group[i]->x_index == x_index && group->group[i]->y_index == y_index - 1) {
                already_in_group = true;
                break;
            }
        }
        if (!already_in_group) {
            get_stone_group(group, x_index, y_index - 1);
        }
    }
}

static u_short get_group_liberties(const Group* group) {
    u_short liberties = 0;
    for (size_t i = 0; i < group->amount; i++) {
        u_char x_index = group->group[i]->x_index;
        u_char y_index = group->group[i]->y_index;

        int board_array_index = x_index + y_index * (int) sqrt(BOARD_SIZE);

        /* Checking if a stone is on the edge of the board and the adjacent intersections are empty */
        /* right edge stone */
        if ((x_index % ((u_char) sqrt(BOARD_SIZE) - 1) != 0 || x_index == 0) && board_array[board_array_index + 1] == EMPTY) {
            liberties++;
        }
        /* left edge stone */
        if (x_index % (u_char) sqrt(BOARD_SIZE) != 0 && board_array[board_array_index - 1] == EMPTY) {
            liberties++;
        }
        /* bottom edge stone */
        if ((y_index % ((u_char) sqrt(BOARD_SIZE) - 1) != 0 || y_index == 0) && board_array[board_array_index + (int) sqrt(BOARD_SIZE)] == EMPTY) {
            liberties++;
        }
        /* top edge stone */
        if (y_index % (u_char) sqrt(BOARD_SIZE) != 0 && board_array[board_array_index - (u_char) sqrt(BOARD_SIZE)] == EMPTY) {
            liberties++;
        }
    }

    return liberties;
}

static void capture_group(GtkWidget *board_grid, const Group* group, StoneType stone_color) {
    if (stone_color == EMPTY) {
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
    if (stone_color == BLACK_STONE) {
        captured_black += (int) group->amount;
    } else if (stone_color == WHITE_STONE) {
        captured_white += (int) group->amount;
    }
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
    if (((u_char*)board_array)[board_array_index] == EMPTY) {
        GtkWidget *placing_stone;
        ((u_char*)board_array)[board_array_index] = current_color;

        GtkWidget *image = gtk_grid_get_child_at(GTK_GRID(board_grid), x_index, y_index);
        gtk_grid_remove(GTK_GRID(board_grid), image);
        if (current_color == BLACK_STONE) {
            placing_stone = gtk_image_new_from_file(BLACK_STONE_PATH);
            current_color = WHITE_STONE;
        } else if (current_color == WHITE_STONE) {
            placing_stone = gtk_image_new_from_file(WHITE_STONE_PATH);
            current_color = BLACK_STONE;
        } else {
            // sentinel("Incorrect stone placement")
            return;
        }

        gtk_image_set_pixel_size(GTK_IMAGE(placing_stone), STONE_SIZE);
        gtk_grid_attach(GTK_GRID(board_grid), placing_stone, x_index, y_index, 1, 1);

        Group group;
        init_group(&group, AVERAGE_GROUP_SIZE);
        get_stone_group(&group, x_index, y_index);

        // TODO: Rewrite necessary because this is duplicate code + ko
        if (get_group_liberties(&group) == 0) {
            g_print("You can't play there!\n");
            ((u_char*)board_array)[board_array_index] = EMPTY;
            image = gtk_grid_get_child_at(GTK_GRID(board_grid), x_index, y_index);
            gtk_grid_remove(GTK_GRID(board_grid), image);
            GtkWidget *empty = gtk_image_new_from_file(EMPTY_PATH);

            gtk_image_set_pixel_size(GTK_IMAGE(empty), STONE_SIZE);
            gtk_grid_attach(GTK_GRID(board_grid), empty, x_index, y_index, 1, 1);

            if (current_color == BLACK_STONE) {
                current_color = WHITE_STONE;
            } else if (current_color == WHITE_STONE) {
                current_color = BLACK_STONE;
            }
            free_group(&group);
            return;
        }

        free_group(&group);

        /* Capture groups that don't have any liberties left.
         * Here, current_color is already updated, so this refers to the opponent color.
         */

        if (((u_char*)board_array)[board_array_index + 1] == current_color) {
            Group opponent_group;
            init_group(&opponent_group, AVERAGE_GROUP_SIZE);
            get_stone_group(&opponent_group, x_index + 1, y_index);
            if (get_group_liberties(&opponent_group) == 0) {
                capture_group(board_grid, &opponent_group, current_color);
            }
            free_group(&opponent_group);
        }
        if (((u_char*)board_array)[board_array_index - 1] == current_color) {
            Group opponent_group;
            init_group(&opponent_group, AVERAGE_GROUP_SIZE);
            get_stone_group(&opponent_group, x_index - 1, y_index);
            if (get_group_liberties(&opponent_group) == 0) {
                capture_group(board_grid, &opponent_group, current_color);
            }
            free_group(&opponent_group);
        }
        if (((u_char*)board_array)[board_array_index + (u_char) sqrt(BOARD_SIZE)] == current_color) {
            Group opponent_group;
            init_group(&opponent_group, AVERAGE_GROUP_SIZE);
            get_stone_group(&opponent_group, x_index, y_index + 1);
            if (get_group_liberties(&opponent_group) == 0) {
                capture_group(board_grid, &opponent_group, current_color);
            }
            free_group(&opponent_group);
        }
        if (((u_char*)board_array)[board_array_index - (u_char) sqrt(BOARD_SIZE)] == current_color) {
            Group opponent_group;
            init_group(&opponent_group, AVERAGE_GROUP_SIZE);
            get_stone_group(&opponent_group, x_index, y_index - 1);
            if (get_group_liberties(&opponent_group) == 0) {
                capture_group(board_grid, &opponent_group, current_color);
            }
            free_group(&opponent_group);
        }
    }
}