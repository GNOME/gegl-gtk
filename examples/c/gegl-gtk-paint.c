/* This file is part of GEGL-GTK
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/>.
 *
 * Copyright (C) 2003, 2004, 2006, 2008 Øyvind Kolås
 * Copyright (C) 2011 Jon Nordby <jononor@gmail.com>
 */

#include <string.h>
#include <glib.h>
#include <gegl.h>
#include <gtk/gtk.h>
#include <gegl-gtk.h>

#define HARDNESS     0.6
#define LINEWIDTH   60.0
#define COLOR       "rgba(0.0,0.0,0.0,0.4)"

GtkWidget         *window;
GtkWidget         *view;
GtkWidget         *eventbox;
static GeglBuffer *buffer   = NULL;
static GeglNode   *gegl     = NULL;
static GeglNode   *out      = NULL;
static GeglNode   *top      = NULL;
static gboolean    pen_down = FALSE;
static GeglPath   *vector   = NULL;

static GeglNode   *over     = NULL;
static GeglNode   *stroke   = NULL;

/* Transform the input coordinate from view coordinates to model coordinates
+ * Returns TRUE if the transformation was successfull, else FALSE */
gboolean
transform_view_to_model_coordinate(gdouble *x, gdouble *y)
{
    GeglMatrix3 matrix;
    gegl_gtk_view_get_transformation(GEGL_GTK_VIEW(view), &matrix);

    if (gegl_matrix3_determinant(&matrix) == 0.0) {
        return FALSE;
    }

    gegl_matrix3_invert(&matrix);
    gegl_matrix3_transform_point(&matrix, x, y);

    return TRUE;
}

static gboolean paint_press(GtkWidget      *widget,
                            GdkEventButton *event)
{
    gdouble x = event->x;
    gdouble y = event->y;
    transform_view_to_model_coordinate(&x, &y);

    if (event->button == 1) {
        vector     = gegl_path_new();

        over       = gegl_node_new_child(gegl, "operation", "gegl:over", NULL);
        stroke     = gegl_node_new_child(gegl, "operation", "gegl:path",
                                         "d", vector,
                                         "fill-opacity", 0.0,
                                         "stroke", gegl_color_new(COLOR),
                                         "stroke-width", LINEWIDTH,
                                         "stroke-hardness", HARDNESS,
                                         NULL);
        gegl_node_link_many(top, over, out, NULL);
        gegl_node_connect_to(stroke, "output", over, "aux");
        gegl_path_append(vector, 'M', x, y);

        pen_down = TRUE;

        return TRUE;
    }
    return FALSE;
}


static gboolean paint_motion(GtkWidget      *widget,
                             GdkEventMotion *event)
{
    gdouble x = event->x;
    gdouble y = event->y;

    transform_view_to_model_coordinate(&x, &y);

    if (event->state & GDK_BUTTON1_MASK) {
        if (!pen_down) {
            return TRUE;
        }

        gegl_path_append(vector, 'L', x, y);
        return TRUE;
    }
    return FALSE;
}


static gboolean paint_release(GtkWidget      *widget,
                              GdkEventButton *event)
{
    if (event->button == 1) {
        gdouble        x0, x1, y0, y1;
        GeglProcessor *processor;
        GeglNode      *writebuf;
        GeglRectangle  roi;

        gegl_path_get_bounds(vector, &x0, &x1, &y0, &y1);

        roi.x = x0 - LINEWIDTH;
        roi.y = y0 - LINEWIDTH;
        roi.width = x1 - x0 + LINEWIDTH * 2;
        roi.height = y1 - y0 + LINEWIDTH * 2;

        writebuf = gegl_node_new_child(gegl,
                                       "operation", "gegl:write-buffer",
                                       "buffer",    buffer,
                                       NULL);
        gegl_node_link_many(over, writebuf, NULL);

        processor = gegl_node_new_processor(writebuf, &roi);
        while (gegl_processor_work(processor, NULL)) ;

        g_object_unref(processor);
        g_object_unref(writebuf);

        gegl_node_link_many(top, out, NULL);
        g_object_unref(over);
        g_object_unref(stroke);

        over     = NULL;
        stroke   = NULL;
        pen_down = FALSE;

        return TRUE;
    }
    return FALSE;
}

static void
draw_overlay(GeglGtkView *view, cairo_t *cr, GdkRectangle *rect)
{
    cairo_translate(cr, 200.0, 200.0);

    cairo_set_source_rgba(cr, 0.0, 0.0, 0.0, 0.3);
    cairo_rectangle(cr, 0.0, 0.0, 200.0, 50.0);
    cairo_fill(cr);

    cairo_set_source_rgba(cr, 0.0, 0.0, 0.0, 0.9);
    cairo_translate(cr, 20.0, 20.0);
    cairo_show_text(cr, "canvas overlay");
    cairo_fill(cr);
}

static void
draw_background(GeglGtkView *view, cairo_t *cr, GdkRectangle *rect)
{
    cairo_set_source_rgba(cr, 0.9, 0.9, 1.0, 1.0);
    cairo_rectangle(cr, 0.0, 0.0, rect->width, rect->height);
    cairo_fill(cr);
}

gint
main(gint    argc,
     gchar **argv)
{
    gtk_init(&argc, &argv);
    gegl_init(&argc, &argv);

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "GEGL-GTK paint example");

    if (argv[1] == NULL) {
        GeglRectangle rect = {0, 0, 512, 512};
        gpointer buf;

        /* XXX: for best overall performance, this format should probably
         * be RaGaBaA float, overeager in-place processing code makes that fail though.
         */
        buffer = gegl_buffer_new(&rect, babl_format("R'G'B' u8"));
        /* it would be useful to have a programmatic way of doing this, filling
         * with a given pixel value
         */
        buf    = gegl_buffer_linear_open(buffer, NULL, NULL, babl_format("Y' u8"));
        memset(buf, 255, 512 * 512);
        gegl_buffer_linear_close(buffer, buf);
    } else {
        buffer = gegl_buffer_open(argv[1]);
    }

    gegl = gegl_node_new();
    {
        GeglNode *loadbuf = gegl_node_new_child(gegl, "operation", "gegl:buffer-source", "buffer", buffer, NULL);
        out  = gegl_node_new_child(gegl, "operation", "gegl:nop", NULL);

        gegl_node_link_many(loadbuf, out, NULL);

        view = GTK_WIDGET(gegl_gtk_view_new_for_node(out));
        gegl_gtk_view_set_x(GEGL_GTK_VIEW(view), -50.0);
        gegl_gtk_view_set_y(GEGL_GTK_VIEW(view), -50.0);
        gegl_gtk_view_set_autoscale_policy(GEGL_GTK_VIEW(view), GEGL_GTK_VIEW_AUTOSCALE_DISABLED);
        top  = loadbuf;
    }


    g_signal_connect(G_OBJECT(view), "draw-overlay",
                     (GCallback) draw_overlay, NULL);
    g_signal_connect(G_OBJECT(view), "draw-background",
                     (GCallback) draw_background, NULL);

    eventbox = gtk_event_box_new();

    g_signal_connect(G_OBJECT(eventbox), "motion-notify-event",
                     (GCallback) paint_motion, NULL);
    g_signal_connect(G_OBJECT(eventbox), "button-press-event",
                     (GCallback) paint_press, NULL);
    g_signal_connect(G_OBJECT(eventbox), "button-release-event",
                     (GCallback) paint_release, NULL);
    gtk_widget_add_events(eventbox, GDK_BUTTON_RELEASE_MASK);

    gtk_container_add(GTK_CONTAINER(eventbox), view);
    gtk_container_add(GTK_CONTAINER(window), eventbox);
    gtk_widget_set_size_request(view, 512, 512);

    g_signal_connect(G_OBJECT(window), "delete-event",
                     G_CALLBACK(gtk_main_quit), window);
    gtk_widget_show_all(window);

    gtk_main();
    g_object_unref(gegl);
    g_object_unref(buffer);

    gegl_exit();
    return 0;
}
