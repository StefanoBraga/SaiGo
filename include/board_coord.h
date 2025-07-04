/* Needs types.h to be included. */
struct BoardCoord {
    u_char x_index;
    u_char y_index;
};

struct BoardCoord *create_board_coord(u_char x_index, u_char y_index);
