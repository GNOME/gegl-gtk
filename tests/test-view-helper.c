
#include <string.h>

#include <glib.h>
#include <gegl.h>

#include <internal/view-helper.h>
#include "utils.c"

/* Stores the state used in widget tests.*/
typedef struct {
    ViewHelper *helper;
    GeglNode *graph, *out, *loadbuf;
    GeglBuffer *buffer;
} ViewHelperTest;


static void
setup_helper_test(ViewHelperTest *test)
{
    gpointer buf;
    GeglRectangle rect = {0, 0, 512, 512};

    /* Create a buffer, fill it with white */
    test->buffer = gegl_buffer_new(&rect, babl_format("R'G'B' u8"));
    buf = gegl_buffer_linear_open(test->buffer, NULL, NULL, babl_format("Y' u8"));
    memset(buf, 255, rect.width * rect.height);
    gegl_buffer_linear_close(test->buffer, buf);

    /* Setup a graph with two nodes, one sourcing the buffer and a no-op */
    test->graph = gegl_node_new();
    test->loadbuf = gegl_node_new_child(test->graph,
                                        "operation", "gegl:buffer-source",
                                        "buffer", test->buffer, NULL);
    test->out  = gegl_node_new_child(test->graph, "operation", "gegl:nop", NULL);
    gegl_node_link_many(test->loadbuf, test->out, NULL);

    /* Setup the GeglView helper, hook up the output node to it */
    test->helper = view_helper_new();
    view_helper_set_node(test->helper, test->out);
}

static void
teardown_helper_test(ViewHelperTest *test)
{
    g_object_unref(test->graph);
    gegl_buffer_destroy(test->buffer);
    g_object_unref(test->helper);
}


typedef struct {
    gboolean needs_redraw_called;
    GeglRectangle *expected_result;
} RedrawTestState;

static void
needs_redraw_event(ViewHelper *helper,
                   GeglRectangle *rect,
                   RedrawTestState *data)
{
    data->needs_redraw_called = TRUE;

    g_assert(test_utils_compare_rect(rect, data->expected_result));
}

/* Test that the redraw signal is emitted when the GeglNode has been computed.
 *
 * NOTE: Does not test that the actual drawing happens, or even
 * that queue_redraw is called, as this is hard to observe reliably
 * Redraws can be triggered by other things, and the exposed events
 * can be coalesced. */
static void
test_redraw_on_computed(void)
{
    ViewHelperTest test;
    GeglRectangle computed_rect = {0, 0, 128, 128};
    RedrawTestState test_data;
    test_data.needs_redraw_called = FALSE;
    test_data.expected_result = &computed_rect;

    setup_helper_test(&test);
    /* Setup will invalidate the node, make sure those events are processed. */
    while (gtk_events_pending()) {
        gtk_main_iteration();
    }
    gegl_node_process(test.out);

    g_assert(IS_VIEW_HELPER(test.helper));

    /* TODO: when adding tests for transformed cases,
     * split out a function for testing the redrawn area, given
     * the input area and the transformation (translation, scaling, rotation) */
    g_signal_connect(G_OBJECT(test.helper), "redraw-needed",
                     G_CALLBACK(needs_redraw_event),
                     &test_data);


    g_signal_emit_by_name(test.out, "computed", &computed_rect, NULL);

    g_timeout_add(300, test_utils_quit_gtk_main, NULL);
    gtk_main();

    g_assert(test_data.needs_redraw_called);

    teardown_helper_test(&test);
}

int
main(int argc, char **argv)
{

    int retval = -1;

    g_thread_init(NULL);
    gegl_init(&argc, &argv);
    g_test_init(&argc, &argv, NULL);

    g_test_add_func("/widgets/view/helper/redraw-on-computed", test_redraw_on_computed);

    retval = g_test_run();
    gegl_exit();
    return retval;
}
