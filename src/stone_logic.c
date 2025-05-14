//
// Created by stefan on 8/05/25.
//

#include "stone_logic.h"
#include "groups.h"

int current_color = BLACK_STONE;

static void get_stone_group(Group* group, const int* board_array, int x_index, int y_index) {
    int board_array_index = x_index + y_index * (int) sqrt(BOARD_SIZE);
    bool right_edge_stone = x_index % ((int) sqrt(BOARD_SIZE) - 1) == 0;
    bool left_edge_stone = x_index % (int) sqrt(BOARD_SIZE) == 0;
    bool bottom_edge_stone = y_index % ((int) sqrt(BOARD_SIZE) - 1) == 0;
    bool top_edge_stone = y_index % (int) sqrt(BOARD_SIZE) == 0;
    StoneType placed_stone_color = board_array[x_index + y_index * (int) sqrt(BOARD_SIZE)];

    BoardCoord *board_coord = malloc(sizeof(BoardCoord));
    board_coord->x_index = x_index;
    board_coord->y_index = y_index;
    insert_group(group, board_coord);

    if (board_array[board_array_index + 1] == placed_stone_color && !right_edge_stone) {
        bool already_in_group = false;
        for (int i = 0; i < group->amount; i++) {
            if (group->group[i]->x_index == x_index + 1 && group->group[i]->y_index == y_index) {
                already_in_group = true;
                break;
            }
        }
        if (!already_in_group) {
            get_stone_group(group, board_array, x_index + 1, y_index);
        }
    }
    if (board_array[board_array_index - 1] == placed_stone_color && !left_edge_stone) {
        bool already_in_group = false;
        for (int i = 0; i < group->amount; i++) {
            if (group->group[i]->x_index == x_index - 1 && group->group[i]->y_index == y_index) {
                already_in_group = true;
                break;
            }
        }
        if (!already_in_group) {
            get_stone_group(group, board_array, x_index - 1, y_index);
        }
    }
    if (board_array[board_array_index + (int) sqrt(BOARD_SIZE)] == placed_stone_color && !bottom_edge_stone) {
        bool already_in_group = false;
        for (int i = 0; i < group->amount; i++) {
            if (group->group[i]->x_index == x_index && group->group[i]->y_index == y_index + 1) {
                already_in_group = true;
                break;
            }
        }
        if (!already_in_group) {
            get_stone_group(group, board_array, x_index, y_index + 1);
        }
    }
    if (board_array[board_array_index - (int) sqrt(BOARD_SIZE)] == placed_stone_color && !top_edge_stone) {
        bool already_in_group = false;
        for (int i = 0; i < group->amount; i++) {
            if (group->group[i]->x_index == x_index && group->group[i]->y_index == y_index - 1) {
                already_in_group = true;
                break;
            }
        }
        if (!already_in_group) {
            get_stone_group(group, board_array, x_index, y_index - 1);
        }
    }
}

static int get_group_liberties(Group* group, const int* board_array) {
    int liberties = 0;
    for (size_t i = 0; i < group->amount; i++) {
        int x_index = group->group[i]->x_index;
        int y_index = group->group[i]->y_index;

        // checking if a stone is on the edge of the board
        bool right_edge_stone = x_index % ((int) sqrt(BOARD_SIZE) - 1) == 0;
        bool left_edge_stone = x_index % (int) sqrt(BOARD_SIZE) == 0;
        bool bottom_edge_stone = y_index % ((int) sqrt(BOARD_SIZE) - 1) == 0;
        bool top_edge_stone = y_index % (int) sqrt(BOARD_SIZE) == 0;

        int board_array_index = x_index + y_index * (int) sqrt(BOARD_SIZE);

        // if (right_edge_stone ||
        // left_edge_stone ||
        // bottom_edge_stone ||
        // top_edge_stone) {
        //     liberties--;
        // }

        if (!right_edge_stone && board_array[board_array_index + 1] == EMPTY) {
            liberties++;
        }
        if (!left_edge_stone && board_array[board_array_index - 1] == EMPTY) {
            liberties++;
        }
        if (!bottom_edge_stone && board_array[board_array_index + (int) sqrt(BOARD_SIZE)] == EMPTY) {
            liberties++;
        }
        if (!top_edge_stone && board_array[board_array_index - (int) sqrt(BOARD_SIZE)] == EMPTY) {
            liberties++;
        }
    }

    return liberties;
}

void play_stone(GtkGestureClick *gesture, gint n_press, gdouble x, gdouble y, gpointer board_array) {
    GtkWidget *board_grid = gtk_event_controller_get_widget(GTK_EVENT_CONTROLLER(gesture));

    int x_index = 0;
    for (int i = 0; i < gtk_widget_get_width(board_grid); i++) {
        if (i * STONE_SIZE + (i - 1) * STONE_SPACING > x) {
            x_index = i - 1;
            break;
        }
    }

    int y_index = 0;
    for (int i = 0; i < gtk_widget_get_height(board_grid); i++) {
        if (i * STONE_SIZE + (i - 1) * STONE_SPACING > y) {
            y_index = i - 1;
            break;
        }
    }

    int board_array_index = x_index + y_index * (int) sqrt(BOARD_SIZE);

    //g_print("x=%f, y=%f, n_press=%d\n", x, y, n_press);
    //g_print("Board index: %d\nBoard value:%d\n", board_array_index, ((int*)board_array)[board_array_index]);

    if (((int*)board_array)[board_array_index] == EMPTY) {
        GtkWidget *placing_stone;
        ((int*)board_array)[board_array_index] = current_color;

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


        //g_print("%d\n", get_liberties(board_array, x_index, y_index););

        Group group;
        init_group(&group, 5);
        get_stone_group(&group, board_array, x_index, y_index);
        g_print("Number of stones in group: %lo\n", group.amount);
        g_print("Number of liberties in group: %d\n", get_group_liberties(&group, board_array));

        free_group(&group);
    }
}