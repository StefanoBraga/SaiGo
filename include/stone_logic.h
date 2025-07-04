/* Needs types.h and <gtk/gtk.h> to be included. */
typedef enum StoneType {
    EMPTY = 0,
    BLACK_STONE = 1,
    WHITE_STONE = 2,
} StoneType;

/* Callbacks */
void play_stone(GtkGestureClick *gesture, gint n_press, gdouble x, gdouble y, gpointer user_data);
void pass(GtkButton* button, gpointer user_data);

/* Normal functions */
bool check_valid_play(u_char x_index, u_char y_index, u_char x_index_that_captured, u_char y_index_that_captured, StoneType stone_type);
StoneType invert_stone_type(StoneType stone_type);
