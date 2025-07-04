/* Needs <gtk/gtk.h> to be included. */
struct Settings {
    GObject *window;
    const char *black_name;
    const char *white_name;
    const char *scoring;
    float komi;
};
