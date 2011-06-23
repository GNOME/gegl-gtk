
#include <glib.h>

#include <gegl-gtk-view.h>
#include <gegl.h>

static gboolean
quit_gtk_main (gpointer data)
{
    gtk_main_quit();
}

/* Test that instantiating the widget and hooking up a gegl graph 
 * does not cause any crashes, criticals or warnings. */
static void
test_sanity (void) {

    GtkWidget *window, *view;
    GeglNode *gegl, *out, *loadbuf;
    GeglBuffer *buffer;
    gpointer buf;
    GeglRectangle rect = {0, 0, 512, 512};

    buffer = gegl_buffer_new (&rect, babl_format("R'G'B' u8"));
    buf = gegl_buffer_linear_open (buffer, NULL, NULL, babl_format ("Y' u8"));
    memset (buf, 255, 512 * 512);
    gegl_buffer_linear_close (buffer, buf);

    gegl = gegl_node_new ();
    loadbuf = gegl_node_new_child (gegl, "operation", "gegl:buffer-source", "buffer", buffer, NULL);
    out  = gegl_node_new_child (gegl, "operation", "gegl:nop", NULL);
    gegl_node_link_many (loadbuf, out, NULL);

    window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    view = GTK_WIDGET (g_object_new (GEGL_GTK_TYPE_VIEW, "node", out, NULL));
    gtk_container_add (GTK_CONTAINER (window), view);
    gtk_widget_set_size_request (view, 512, 512);
    gtk_widget_show_all (window);

    // XXX: Better to do on/after expose event instead?
    g_timeout_add(300, quit_gtk_main, NULL);
    gtk_main ();

    g_object_unref (gegl);
    gegl_buffer_destroy (buffer);
    gtk_widget_destroy (window);
}

/* TODO:
 * - Test redraw logic
 * - Test redraw with translation
 * - Test redraw with scaling
 * - Test redraw with rotation
 * Benchmarks for cases above
 */

int
main (int argc, char **argv) {

    int retval = -1;

    g_thread_init(NULL);
    gtk_init(&argc, &argv);
    gegl_init(&argc, &argv);
    g_test_init(&argc, &argv, NULL);

    g_test_add_func("/widgets/view/sanity", test_sanity);

    retval = g_test_run();
    gegl_exit();
    return retval;
}
