
#include <glib.h>

#include <gegl-gtk-view.h>
#include <gegl.h>

gboolean
test_utils_display_is_set () {
  return g_getenv ("DISPLAY");
}

void
test_utils_print_rect (GeglRectangle *rect) {

    g_print ("GeglRectangle: %d,%d %dx%d", rect->x, rect->y, rect->width, rect->height);
}

static gboolean
test_utils_quit_gtk_main (gpointer data)
{
    gtk_main_quit();
}

/* Compare two rectangles, output */
gboolean
test_utils_compare_rect (GeglRectangle *r, GeglRectangle *s)
{
    gboolean equal = gegl_rectangle_equal (r, s);
    if (!equal) {
        test_utils_print_rect (r);
        g_printf ("%s", " != ");
        test_utils_print_rect (s);

    }
    return equal;
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


static void
computed_event (GeglNode      *node,
                GeglRectangle *rect,
                gpointer       data)
{
    gboolean *got_computed = (gboolean *)data;
    *got_computed = TRUE;
}

/* Test that the GeglNode is processed when invalidated. */
static void
test_processing (void)
{
    ViewWidgetTest test;
    gboolean got_computed_event = FALSE;
    GeglRectangle invalidated_rect = {0, 0, 128, 128};

    setup_widget_test(&test);
    /* Setup will invalidate the node, make sure those events are processed. */
    while (gtk_events_pending ()) {
        gtk_main_iteration ();
    }
    gegl_node_process (test.out);

    g_signal_connect (test.out, "computed",
                      G_CALLBACK (computed_event),
                      &got_computed_event);

    g_signal_emit_by_name (test.out, "invalidated", &invalidated_rect, NULL);

    g_timeout_add (300, test_utils_quit_gtk_main, NULL);
    gtk_main ();

    /* FIXME: test that the computed events span the invalidated area */
    g_assert (got_computed_event);

    teardown_widget_test(&test);
}

typedef struct {
    gboolean needs_redraw_called;
    GeglRectangle *expected_result;
} RedrawTestState;

static void
needs_redraw_event (GeglGtkView *view,
                GeglRectangle *rect,
                RedrawTestState *data)
{
    data->needs_redraw_called = TRUE;

    g_assert (test_utils_compare_rect (rect, data->expected_result));
}

/* Test that the redraw signal is emitted when the GeglNode has been computed.
 *
 * NOTE: Does not test that the actual drawing happens, or even
 * that queue_redraw is called, as this is hard to observe reliably
 * Redraws can be triggered by other things, and the exposed events
 * can be coalesced. */
static void
test_redraw_on_computed (void)
{
    ViewWidgetTest test;
    GeglRectangle computed_rect = {0, 0, 128, 128};
    RedrawTestState test_data;
    test_data.expected_result = &computed_rect;

    setup_widget_test(&test);
    /* Setup will invalidate the node, make sure those events are processed. */
    while (gtk_events_pending ()) {
        gtk_main_iteration ();
    }
    gegl_node_process (test.out);

    g_assert (GEGL_GTK_IS_VIEW (test.view));

    /* TODO: when adding tests for transformed cases,
     * split out a function for testing the redrawn area, given
     * the input area and the transformation (translation, scaling, rotation) */
    g_signal_connect (G_OBJECT (test.view), "redraw",
                      G_CALLBACK (needs_redraw_event),
                      &test_data);



    g_signal_emit_by_name (test.out, "computed", &computed_rect, NULL);

    g_timeout_add (300, test_utils_quit_gtk_main, NULL);
    gtk_main ();

    g_assert(test_data.needs_redraw_called);

    teardown_widget_test(&test);
}

/* TODO:
 * - Test redraw with translation
 * - Test redraw with scaling
 * - Test redraw with rotation
 * Benchmarks for cases above
 * Actual drawing tests, checking the output of the widget against a
 * well known reference. Ideally done with a fake/dummy windowing backend,
 * so it can be done quickly, without external influences.
 */

/* Note that ideally only a few tests requires setting up a mainloop, and
having a real widget backend present. */

int
main (int argc, char **argv) {

    int retval = -1;

    /* Currently all tests depend on having a display server */
    if (!test_utils_display_is_set()) {
        g_printf("%s", "Warning: Skipping tests due to missing display server.\n");
        exit(0);
    }

    g_thread_init(NULL);
    gtk_init(&argc, &argv);
    gegl_init(&argc, &argv);
    g_test_init(&argc, &argv, NULL);

    g_test_add_func("/widgets/view/sanity", test_sanity);
    g_test_add_func("/widgets/view/processing", test_processing);
    g_test_add_func("/widgets/view/redraw-on-computed", test_redraw_on_computed);

    retval = g_test_run();
    gegl_exit();
    return retval;
}
