//
// Created by stefan on 8/05/25.
//

#include "consts.h"
#include "dbg.h"
#include "groups.h"
#include "interface.h"
#include "stone_logic.h"

extern u_char board_array[];
int captured_black = 0;
int captured_white = 0;

int current_color = BLACK_STONE;
u_char n_passes = 0;

static bool
at_right_edge(u_char x_index)
{
    return x_index % ((u_char) sqrt(BOARD_SIZE) - 1) == 0 && x_index != 0;
}

static bool
at_left_edge(u_char x_index)
{
    return x_index % (u_char) sqrt(BOARD_SIZE) == 0;
}

static bool
at_top_edge(u_char y_index)
{
    return y_index % (u_char) sqrt(BOARD_SIZE) == 0;
}

static bool
at_bottom_edge(u_char y_index)
{
    return y_index % ((u_char) sqrt(BOARD_SIZE) - 1) == 0 && y_index != 0;
}

static StoneType
invert_stone_type(StoneType stone_type)
{
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

static void
get_board_indices_from_grid(GtkWidget* board_grid, gdouble x, gdouble y, u_char *x_index, u_char *y_index)
{
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

static void
get_stone_group(Group* group, u_char x_index, u_char y_index)
{
    int board_array_index = x_index + y_index * (int) sqrt(BOARD_SIZE);

    StoneType placed_stone_color = board_array[x_index + y_index * (int) sqrt(BOARD_SIZE)];

    if (placed_stone_color == EMPTY) {
        return;
    }

    struct BoardCoord *board_coord = create_board_coord(x_index, y_index);
    insert_group(group, board_coord);

    /* TODO: rewrite, duplicate code */
    /* Checking if a stone is on the edge of the board */
    if (board_array[board_array_index + 1] == placed_stone_color && !at_right_edge(x_index)) {
        if (!group_contains_indices(group, x_index + 1, y_index)) {
            get_stone_group(group, x_index + 1, y_index);
        }
    }
    if (board_array[board_array_index - 1] == placed_stone_color && !at_left_edge(x_index)) {
        if (!group_contains_indices(group, x_index - 1, y_index)) {
            get_stone_group(group, x_index - 1, y_index);
        }
    }
    if (board_array[board_array_index + (u_char) sqrt(BOARD_SIZE)] == placed_stone_color && !at_bottom_edge(y_index)) {
        if (!group_contains_indices(group, x_index, y_index + 1)) {
            get_stone_group(group, x_index, y_index + 1);
        }
    }
    if (board_array[board_array_index - (u_char) sqrt(BOARD_SIZE)] == placed_stone_color && !at_top_edge(y_index)) {
        if (!group_contains_indices(group, x_index, y_index - 1)) {
            get_stone_group(group, x_index, y_index - 1);
        }
    }
}

static u_short
get_group_liberties(const Group* group)
{
    u_short liberties = 0;
    Group counted_liberties;
    init_group(&counted_liberties, AVERAGE_GROUP_SIZE);
    for (size_t i = 0; i < group->amount; i++) {
        u_char x_index = group->group[i]->x_index;
        u_char y_index = group->group[i]->y_index;

        int board_array_index = x_index + y_index * (int) sqrt(BOARD_SIZE);

        /* Checking if a stone is on the edge of the board and the adjacent intersections are empty */
        /* right edge stone */
        if (!at_right_edge(x_index) && board_array[board_array_index + 1] == EMPTY) {
            if (!group_contains_indices(&counted_liberties, x_index + 1, y_index)) {
                struct BoardCoord* board_coord = create_board_coord(x_index + 1, y_index);
                insert_group(&counted_liberties, board_coord);
                liberties++;
            }
        }
        /* left edge stone */
        if (!at_left_edge(x_index) && board_array[board_array_index - 1] == EMPTY) {
            if (!group_contains_indices(&counted_liberties, x_index - 1, y_index)) {
                struct BoardCoord* board_coord = create_board_coord(x_index - 1, y_index);
                insert_group(&counted_liberties, board_coord);
                liberties++;
            }
        }
        /* bottom edge stone */
        if (!at_bottom_edge(y_index) && board_array[board_array_index + (int) sqrt(BOARD_SIZE)] == EMPTY) {
            if (!group_contains_indices(&counted_liberties, x_index, y_index + 1)) {
                struct BoardCoord* board_coord = create_board_coord(x_index, y_index + 1);
                insert_group(&counted_liberties, board_coord);
                liberties++;
            }
        }
        /* top edge stone */
        if (!at_top_edge(y_index) && board_array[board_array_index - (u_char) sqrt(BOARD_SIZE)] == EMPTY) {
            if (!group_contains_indices(&counted_liberties, x_index, y_index - 1)) {
                struct BoardCoord* board_coord = create_board_coord(x_index, y_index - 1);
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
static void
capture_group(GtkWidget *board_grid, const Group* group, StoneType stone_type)
{
    if (stone_type == EMPTY) {
        log_err("stone_color is EMPTY_STONE. Can't capture empty group.");
        return;
    }
    for (size_t i = 0; i < group->amount; i++) {
        struct BoardCoord* board_coord = group->group[i];
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
static void
get_captured_groups(Group *groups[], u_char x_index, u_char y_index, StoneType stone_type)
{
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
}

bool
check_valid_play(
    u_char x_index, u_char y_index, u_char x_index_that_captured, u_char y_index_that_captured, StoneType stone_type)
{
    Group group;
    int board_array_index;
    bool capturable, ko;


    init_group(&group, AVERAGE_GROUP_SIZE);

    /* 'Image' a stone being on the x and y location and add it to the group */
    board_array_index = x_index + y_index * (int) sqrt(BOARD_SIZE);
    StoneType current_stone = board_array[board_array_index];
    board_array[board_array_index] = stone_type;

    get_stone_group(&group, x_index, y_index);

    Group *captured_groups[4];
    get_captured_groups(captured_groups, x_index, y_index, invert_stone_type(stone_type));

    capturable = ko = false;
    for (u_char i = 0; i < 4; i++) {
        const Group *gp = captured_groups[i];
        if (gp == NULL) {
            continue;
        }
        const struct BoardCoord * bcp = gp->group[0];

        if (gp->amount == 1 && bcp->x_index == x_index_that_captured && bcp->y_index == y_index_that_captured) {
            ko = true;
        }

        capturable = true;
        free_group(captured_groups[i]);
        free(captured_groups[i]);
    }

    if ((get_group_liberties(&group) == 0 && !capturable) || ko) {
        free_group(&group);
        board_array[board_array_index] = current_stone;
        return false;
    }
    free_group(&group);
    board_array[board_array_index] = current_stone;

    return true;
}

void
play_stone(GtkGestureClick *gesture, gint n_press, gdouble x, gdouble y, gpointer user_data)
{
    static u_char x_index_that_captured = 255;
    static u_char y_index_that_captured = 255;
    n_passes = 0;

    GtkWidget *board_grid = gtk_event_controller_get_widget(GTK_EVENT_CONTROLLER(gesture));
    u_char x_index;
    u_char y_index;

    /* Use gtk_gesture_single_get_current_button() to detect right clicks */
    // g_print("Button number: %d\n", gtk_gesture_single_get_current_button(GTK_GESTURE_SINGLE(gesture)));

    /* Get x and y index on the board */
    get_board_indices_from_grid(board_grid, x, y, &x_index, &y_index);

    int board_array_index = x_index + y_index * (int) sqrt(BOARD_SIZE);

    /* Check if the point is empty and play the stone */
    if (((u_char*)board_array)[board_array_index] == EMPTY &&
        check_valid_play(x_index, y_index, x_index_that_captured, y_index_that_captured, current_color)) {
        ((u_char*)board_array)[board_array_index] = current_color;

        x_index_that_captured = 255;
        y_index_that_captured = 255;

        add_stone_to_board(board_grid, x_index, y_index, current_color);
        current_color = invert_stone_type(current_color);

        /*
         * Capture groups that don't have any liberties left.
         * Here, current_color is already updated, so this refers to the opponent color.
         */
        Group *captured_groups[4];
        get_captured_groups(captured_groups, x_index, y_index, current_color);
        for (u_char i = 0; i < 4; i++) {
            if (captured_groups[i] == NULL) continue;
            if (captured_groups[i]->amount == 1) {
                x_index_that_captured = x_index;
                y_index_that_captured = y_index;
            }
            capture_group(board_grid, captured_groups[i], current_color);
            /*
             * First free the 'vector' that contains the BoardCoords,
             * then free the actual group.
             */
            free_group(captured_groups[i]);
            free(captured_groups[i]);
        }
    }
}

void
pass(GtkButton* button, gpointer user_data)
{
    current_color = invert_stone_type(current_color);
    n_passes++;
    if (n_passes == 2) {
        gtk_window_close(GTK_WINDOW(user_data));
    }
}