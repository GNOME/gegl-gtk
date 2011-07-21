
#include <glib.h>

#include <gegl-gtk-view.h>
#include <gegl.h>

static gboolean
test_utils_quit_gtk_main (gpointer data)
{
    gtk_main_quit();
}

/* Stores the state used in widget tests.*/
typedef struct {
    GtkWidget *window, *view;
    GeglNode *graph, *out, *loadbuf;
    GeglBuffer *buffer;
} ViewWidgetTest;

static void
setup_widget_test (ViewWidgetTest *test)
{
    gpointer buf;
    GeglRectangle rect = {0, 0, 512, 512};

    /* Create a buffer, fill it with white */
    test->buffer = gegl_buffer_new (&rect, babl_format("R'G'B' u8"));
    buf = gegl_buffer_linear_open (test->buffer, NULL, NULL, babl_format ("Y' u8"));
    memset (buf, 255, rect.width * rect.height);
    gegl_buffer_linear_close (test->buffer, buf);

    /* Setup a graph with two nodes, one sourcing the buffer and a no-op */
    test->graph = gegl_node_new ();
    test->loadbuf = gegl_node_new_child (test->graph,
            "operation", "gegl:buffer-source",
            "buffer", test->buffer, NULL);
    test->out  = gegl_node_new_child (test->graph, "operation", "gegl:nop", NULL);
    gegl_node_link_many (test->loadbuf, test->out, NULL);

    /* Setup the GeglView widget, and a window for it */
    test->window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    test->view = GTK_WIDGET (g_object_new (GEGL_GTK_TYPE_VIEW, "node", test->out, NULL));
    gtk_container_add (GTK_CONTAINER (test->window), test->view);
    gtk_widget_set_size_request (test->view, rect.width, rect.height);
    gtk_widget_show_all (test->window);
}

static void
teardown_widget_test (ViewWidgetTest *test)
{
    g_object_unref (test->graph);
    gegl_buffer_destroy (test->buffer);
    gtk_widget_destroy (test->window);
}

/* Test that instantiating the widget and hooking up a gegl graph 
 * does not cause any crashes, criticals or warnings. */
static void
test_sanity (void)
{
    ViewWidgetTest test;

    setup_widget_test(&test);

    // XXX: Better to do on/after expose event instead?
    g_timeout_add(300, test_utils_quit_gtk_main, NULL);
    gtk_main ();

    teardown_widget_test(&test);
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
