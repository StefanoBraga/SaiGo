#include <glib/gstdio.h>
/*
* Needed by stone_logic.h, game_logic.h
* and interface.h.
*/
#include <gtk/gtk.h>
#include <stdlib.h>

/*
* Needed by stone_logic.h, game_logic.h
* and interface.h.
*/
#include "types.h"
#include "consts.h"
#include "dbg.h"
#include "game_logic.h"
#include "stone_logic.h" /* Needed by interface.h */
#include "interface.h"

extern struct Settings settings;

static struct SettingFields setting_fields;

static void
accept_settings(GtkButton* button, gpointer user_data)
{
    GtkEntryBuffer *black_buf = gtk_entry_get_buffer(GTK_ENTRY(setting_fields.black_name_entry));
    GtkEntryBuffer *white_buf = gtk_entry_get_buffer(GTK_ENTRY(setting_fields.white_name_entry));
    GtkEntryBuffer *komi_buf = gtk_entry_get_buffer(GTK_ENTRY(setting_fields.komi_entry));
    GtkStringObject *scoring_str =
        gtk_drop_down_get_selected_item(GTK_DROP_DOWN(setting_fields.scoring_dropdown));

    settings.black_name = gtk_entry_buffer_get_text(black_buf);
    settings.white_name = gtk_entry_buffer_get_text(white_buf);
    settings.komi = strtof(gtk_entry_buffer_get_text(komi_buf), NULL);
    settings.scoring = gtk_string_object_get_string(scoring_str);

    g_printf("%s\n", settings.black_name);
    g_printf("%s\n", settings.white_name);
    g_printf("%s\n", settings.scoring);
    g_printf("%f\n", settings.komi);

    gtk_widget_set_sensitive(GTK_WIDGET(settings.window), TRUE);
    gtk_widget_set_visible(GTK_WIDGET(setting_fields.settings_window), FALSE);
}

static void
activate(GtkApplication *app, gpointer user_data)
{
    GObject *window, *board_grid, *pass_button;
    GObject *accept_button;
    GFile *empty_file = g_file_new_for_path(EMPTY_PATH);
    GdkTexture *empty_texture;
    GtkBuilder *builder = gtk_builder_new();

    GtkGesture *click_gesture = gtk_gesture_click_new();
    gtk_gesture_single_set_button(GTK_GESTURE_SINGLE(click_gesture), 0);

    /* Load our UI description */
    gtk_builder_add_from_file(builder, BUILDER_PATH, NULL);

    window = gtk_builder_get_object(builder, "window");
    board_grid = gtk_builder_get_object(builder, "board_grid");
    pass_button = gtk_builder_get_object(builder, "pass_button");

    init_setting_fields(builder, &setting_fields);
    accept_button = gtk_builder_get_object(builder, "accept_button");

    settings.window = window;

    empty_texture = gdk_texture_new_from_file(empty_file, NULL);
    if (empty_texture == NULL) {
        g_print("Error loading empty texture\n");
    }

    gtk_window_set_application(GTK_WINDOW(window), app);

    /* Set up the board. */
    init_board(GTK_GRID(board_grid), empty_texture);

    /* Connecting the signals */
    gtk_widget_add_controller(GTK_WIDGET(board_grid), GTK_EVENT_CONTROLLER(click_gesture));
    g_signal_connect(click_gesture, "pressed", G_CALLBACK(play_stone), NULL);
    g_signal_connect(pass_button, "clicked", G_CALLBACK(pass), NULL);
    g_signal_connect(accept_button, "clicked", G_CALLBACK(accept_settings), NULL);

    gtk_widget_set_sensitive(GTK_WIDGET(window), FALSE);

    gtk_widget_set_visible(GTK_WIDGET(window), TRUE);
    gtk_widget_set_visible(GTK_WIDGET(setting_fields.settings_window), TRUE);

    g_object_unref(empty_file);
    g_object_unref(empty_texture);
    g_object_unref(builder);
}

int
main(int argc, char **argv)
{
    GtkApplication *app;
    int status;

    app = gtk_application_new("com.github.StefanoBraga.SaiGo",
                              0);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    status = g_application_run(G_APPLICATION(app), argc, argv);

    g_object_unref(app);

    return status;
}
