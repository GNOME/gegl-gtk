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
 * Copyright (C) 2011 Michael Muré <batolettre@gmail.com>
 */

#include <string.h>
#include <glib.h>
#include <gegl.h>
#include <gtk/gtk.h>
#include <gegl-gtk.h>

/* gegl */
typedef enum
{
  GEGL_WARP_BEHAVIOR_MOVE,
  GEGL_WARP_BEHAVIOR_GROW,
  GEGL_WARP_BEHAVIOR_SHRINK,
  GEGL_WARP_BEHAVIOR_SWIRL_CW,
  GEGL_WARP_BEHAVIOR_SWIRL_CCW,
  GEGL_WARP_BEHAVIOR_ERASE,
  GEGL_WARP_BEHAVIOR_SMOOTH
} GeglWarpBehavior;

/* Tool */
static gdouble         cursor_x; /* Hold the cursor x position */
static gdouble         cursor_y; /* Hold the cursor y position */

static GeglBuffer     *coords_buffer; /* Gegl buffer where coordinates are stored */

static GeglNode       *graph; /* Top level GeglNode. All others node are child of it */
static GeglNode       *read_coords_buffer_node; /* Gegl node that read in the coords buffer */
static GeglNode       *render_node; /* Gegl node to render the transformation */

static GeglPath       *current_stroke;
static guint           stroke_timer;

#define STROKE_PERIOD 100

/* Tool options */
static gdouble          strength = 100;
static gdouble          size = 40;
static gdouble          hardness = 0.5;
static GeglWarpBehavior behavior = GEGL_WARP_BEHAVIOR_MOVE;

/* gegl-gtk stuff */
GtkWidget               *window;
GtkWidget               *view;
GtkWidget               *eventbox;

static GeglRectangle     rect; /* size for the view/window */
static GeglBuffer       *original_buffer = NULL; /* image to be warped */
static GeglNode         *readbuf; /* node to read this image */

static gboolean
add_event_timer (gpointer data)
{
  gegl_path_append (current_stroke,
                    'L', cursor_x, cursor_y);
  return TRUE;
}

static void
add_op ()
{
  GeglNode *new_op, *last_op;

  new_op = gegl_node_new_child (graph,
                                "operation", "gegl:warp",
                                "behavior", behavior,
                                "strength", strength,
                                "size", size,
                                "hardness", hardness,
                                "stroke", current_stroke,
                                NULL);

  last_op = gegl_node_get_producer (render_node, "aux", NULL);

  gegl_node_disconnect (render_node, "aux");

  gegl_node_connect_to (last_op, "output", new_op, "input");
  gegl_node_connect_to (new_op, "output", render_node, "aux");
}

static gboolean paint_press (GtkWidget      *widget,
                             GdkEventButton *event)
{
  if (current_stroke)
    g_object_unref (current_stroke);

  current_stroke = gegl_path_new ();
  gegl_path_append (current_stroke,
                    'M', event->x, event->y);

  cursor_x = event->x;
  cursor_y = event->y;

  add_op ();

  stroke_timer = g_timeout_add (STROKE_PERIOD, add_event_timer, NULL);
  return TRUE;
}


static gboolean paint_motion (GtkWidget      *widget,
                              GdkEventMotion *event)
{
  cursor_x = event->x;
  cursor_y = event->y;

  return FALSE;
}


static gboolean paint_release (GtkWidget      *widget,
                               GdkEventButton *event)
{
  g_source_remove (stroke_timer);

  return TRUE;
}

static void
create_graph ()
{
  printf ("Initialize coordinate buffer (%d,%d) at %d,%d\n", rect.width, rect.height, rect.x, rect.y);
  coords_buffer = gegl_buffer_new (&rect, babl_format_n (babl_type ("float"), 2));

  graph = gegl_node_new ();

  readbuf = gegl_node_new_child (graph,
                                 "operation", "gegl:buffer-source",
                                 "buffer",    original_buffer,
                                 NULL);

  read_coords_buffer_node = gegl_node_new_child (graph,
                                                 "operation", "gegl:buffer-source",
                                                 "buffer",    coords_buffer,
                                                 NULL);

  render_node = gegl_node_new_child (graph,
                                     "operation", "gegl:map-relative",
                                     NULL);


  gegl_node_connect_to (readbuf, "output",
                        render_node, "input");

  gegl_node_connect_to (read_coords_buffer_node, "output",
                        render_node, "aux");
}

gint
main (gint    argc,
      gchar **argv)
{
  if (argv[1] == NULL)
    {
      printf("usage: %s filename.gegl\n", argv[0]);
      printf("filename.gegl must be a Gegl buffer file, for instance created with the 2geglbuffer example from Gegl.\n");
      return 0;
    }

  gtk_init (&argc, &argv);
  gegl_init (&argc, &argv);

  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (GTK_WINDOW (window), "Warp example");

  original_buffer = gegl_buffer_open (argv[1]);
  rect = *gegl_buffer_get_extent(original_buffer);

  create_graph ();

  view = g_object_new (GEGL_GTK_TYPE_VIEW, "node", render_node, NULL);

  eventbox = gtk_event_box_new ();

  g_signal_connect (G_OBJECT (eventbox), "motion-notify-event",
                    (GCallback) paint_motion, NULL);
  g_signal_connect (G_OBJECT (eventbox), "button-press-event",
                    (GCallback) paint_press, NULL);
  g_signal_connect (G_OBJECT (eventbox), "button-release-event",
                    (GCallback) paint_release, NULL);
  gtk_widget_add_events (eventbox, GDK_BUTTON_RELEASE_MASK);

  gtk_container_add (GTK_CONTAINER (eventbox), view);
  gtk_container_add (GTK_CONTAINER (window), eventbox);
  gtk_widget_set_size_request (view, rect.width, rect.height);

  g_signal_connect (G_OBJECT (window), "delete-event",
                    G_CALLBACK (gtk_main_quit), window);
  gtk_widget_show_all (window);

  gtk_main ();
  g_object_unref (graph);
  g_object_unref (original_buffer);
  g_object_unref (coords_buffer);

  gegl_exit ();
  return 0;
}
