#include <stdlib.h>
#include <gtk/gtk.h>
#include <glib/gstdio.h>
#include "parsers/sgf_parser.h"

static void print_hello(GtkWidget *widget, gpointer data) {
	g_print("Hello!\n");
}

static void activate(GtkApplication *app, gpointer user_data) {
	GtkWidget *window;
	GtkWidget *button;
	/* Construct GtkBuilder instance */
	// GtkBuilder *builder = gtk_builder_new();
	// gtk_builder_add_from_file(builder, "src/builder.ui", NULL);
	
	window = gtk_application_window_new(app);
	gtk_window_set_title(GTK_WINDOW(window), "SaiGo");
	gtk_window_set_default_size (GTK_WINDOW (window), 750, 750);
	
	GtkWidget *grid = gtk_grid_new();
	
	GtkWidget *wood_image = gtk_picture_new_for_filename("images/seamless-wood-background-1-900x900.png");
	GtkWidget *board_image = gtk_picture_new_for_filename("images/board.png");
	gtk_widget_set_halign(board_image, GTK_ALIGN_CENTER);
	gtk_widget_set_valign(board_image, GTK_ALIGN_CENTER);
	
	button = gtk_button_new();
	g_signal_connect(button, "clicked", G_CALLBACK(print_hello), NULL);
	gtk_widget_set_opacity(button, 0);
	
	gtk_grid_attach(GTK_GRID(grid), button, 0, 0, 1, 1);
	
	/* Connect signal handlers to constructed widgets. */
	// GObject *window =  gtk_builder_get_object(builder, "window");
	// gtk_window_set_application(GTK_WINDOW(window), app);
	
	GtkWidget *overlay = gtk_overlay_new();
	gtk_widget_set_halign(overlay, GTK_ALIGN_FILL);
	gtk_widget_set_valign(overlay, GTK_ALIGN_FILL);
	
	gtk_window_set_child(GTK_WINDOW(window), overlay);
	
	gtk_overlay_add_overlay(GTK_OVERLAY(overlay), wood_image);
	gtk_overlay_add_overlay(GTK_OVERLAY(overlay), board_image);
	gtk_overlay_add_overlay(GTK_OVERLAY(overlay), grid);

	
//	GtkWidget* box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
//	gtk_widget_set_halign(box, GTK_ALIGN_CENTER);
//	gtk_widget_set_valign(box, GTK_ALIGN_START);
//	gtk_box_append(GTK_BOX(box), wood_image);
	
	// gtk_widget_set_visible(GTK_WIDGET(window), TRUE);
	
	gtk_window_present(GTK_WINDOW(window));
	
	// g_object_unref(builder);
}

int main(int argc, char **argv) {
//	if (argc < 2) {
//		fprintf(stderr, "Not enough arguments: add a file name\n");
//		exit(EXIT_FAILURE);
//	} 
#ifdef GTK_SRCDIR
	g_chdir(GTK_SRCDIR);
#endif

	GtkApplication *app;
	GtkCssProvider *provider = gtk_css_provider_new();
	GFile *css_gfile = g_file_new_for_path("src/main.css");
	int status;
	
	gtk_css_provider_load_from_file(provider, css_gfile);
	
	app = gtk_application_new("com.github.StefanoBraga.SaiGo", G_APPLICATION_DEFAULT_FLAGS);
	g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
	status = g_application_run(G_APPLICATION(app), argc, argv);
	
	g_object_unref(app);
	
	return status;
}