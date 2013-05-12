/* This file is part of GEGL-GTK
 *
 * GEGL-GTK is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * GEGL-GTK is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with GEGL-GTK; if not, see <http://www.gnu.org/licenses/>.
 *
 * Copyright (C) 2011 Jon Nordby <jononor@gmail.com>
 */

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
    g_object_unref(test->buffer);
    g_object_unref(test->helper);
}


static void
computed_event(GeglNode      *node,
               GeglRectangle *rect,
               gpointer       data)
{
    gboolean *got_computed = (gboolean *)data;
    *got_computed = TRUE;
}

/* Test that the GeglNode is processed when invalidated. */
static void
test_processing(void)
{
    ViewHelperTest test;
    gboolean got_computed_event = FALSE;
    GeglRectangle invalidated_rect = {0, 0, 128, 128};

    setup_helper_test(&test);
    /* Setup will invalidate the node, make sure those events are processed. */
    while (gtk_events_pending()) {
        gtk_main_iteration();
    }
    gegl_node_process(test.out);

    g_signal_connect(test.out, "computed",
                     G_CALLBACK(computed_event),
                     &got_computed_event);

    gegl_node_invalidated(test.out, &invalidated_rect, FALSE);

    g_timeout_add(300, test_utils_quit_gtk_main, NULL);
    gtk_main();

    /* FIXME: test that the computed events span the invalidated area */
    g_assert(got_computed_event);

    teardown_helper_test(&test);
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
 * can be coalesced by GTK. */
static void
test_redraw_on_computed (int x, int y, float scale,
                         GeglRectangle *input, GeglRectangle *output)
{
    ViewHelperTest test;
    RedrawTestState test_data;
    test_data.expected_result = output;

    setup_helper_test(&test);
    /* Setup will invalidate the node, make sure those events are processed. */
    while (gtk_events_pending()) {
        gtk_main_iteration();
    }
    gegl_node_process (test.out);

    view_helper_set_x(test.helper, x);
    view_helper_set_y(test.helper, y);
    view_helper_set_scale(test.helper, scale);

    g_signal_connect(G_OBJECT(test.helper), "redraw-needed",
                      G_CALLBACK(needs_redraw_event),
                      &test_data);

    g_signal_emit_by_name(test.out, "computed", input, NULL);

    g_timeout_add(300, test_utils_quit_gtk_main, NULL);
    gtk_main();

    g_assert(test_data.needs_redraw_called);

    teardown_helper_test(&test);
}

static void
test_redraw_basic()
{
    GeglRectangle computed_rect = {0, 0, 128, 128};
    GeglRectangle redraw_rect = {0, 0, 128, 128};
    test_redraw_on_computed (0, 0, 1.0, &computed_rect, &redraw_rect);
}

static void
test_redraw_translated()
{
    GeglRectangle computed_rect = {0, 0, 128, 128};
    GeglRectangle redraw_rect = {-11, -11, 128, 128};
    test_redraw_on_computed (11, 11, 1.0, &computed_rect, &redraw_rect);
}

static void
test_redraw_scaled()
{
    GeglRectangle computed_rect = {0, 0, 128, 128};
    GeglRectangle redraw_rect = {0, 0, 256, 256};
    test_redraw_on_computed (0, 0, 2.0, &computed_rect, &redraw_rect);
}

static void
test_redraw_combined()
{
    GeglRectangle computed_rect = {0, 0, 128, 128};
    GeglRectangle redraw_rect = {10, 10, 256, 256};
    test_redraw_on_computed (-10, -10, 2.0, &computed_rect, &redraw_rect);
}

int
main(int argc, char **argv)
{

    int retval = -1;

    gegl_init(&argc, &argv);
    g_test_init(&argc, &argv, NULL);

    g_test_add_func("/widgets/view/helper/processing", test_processing);
    g_test_add_func("/widgets/view/redraw-basic", test_redraw_basic);
    g_test_add_func("/widgets/view/redraw-scaled", test_redraw_scaled);
    g_test_add_func("/widgets/view/redraw-translated", test_redraw_translated);
    g_test_add_func("/widgets/view/redraw-combined", test_redraw_combined);

    retval = g_test_run();
    gegl_exit();
    return retval;
}
