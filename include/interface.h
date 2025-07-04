/* Needs <sys/types.h>, <gtk/gtk.h> and stone_logic.h to be included. */
struct SettingFields {
    GObject *settings_window;
    GObject *black_name_entry;
    GObject *white_name_entry;
    GObject *scoring_dropdown;
    GObject *komi_entry;
};

GtkWidget* init_window(GtkApplication* app);
void init_setting_fields(GtkBuilder *builder, struct SettingFields *setting_fields);
void init_board(GtkGrid *board_grid, GdkTexture *empty_texture);
void add_stone_to_board(GtkWidget* board_grid, u_char x_index, u_char y_index, StoneType stone_type);
