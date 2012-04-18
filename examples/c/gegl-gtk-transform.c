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
 * Copyright (C) 2011 Jon Nordby <jononor@gmail.com>
 */

#include <string.h>
#include <glib.h>
#include <gegl.h>
#include <gtk/gtk.h>
#include <gegl-gtk.h>

static gboolean motion_event (GtkWidget      *widget,
                              GdkEventMotion *event,
			      GeglGtkView    *view)
{

  static gboolean has_last_pos = FALSE;
  static gfloat last_x, last_y;

  if (event->state & GDK_BUTTON1_MASK) {
    if (has_last_pos) { 
      
      g_object_set (view, "x", -last_x, "y", -last_y, NULL);
    }

    last_x = event->x;
    last_y = event->y;
    has_last_pos = TRUE;
  }


  return FALSE;
}

gint
main (gint    argc,
      gchar **argv)
{
  GtkWidget *window = NULL;
  GtkWidget *view = NULL;
  GtkWidget *eventbox = NULL;
  GeglNode *graph = NULL;
  GeglNode *node = NULL;

  gtk_init (&argc, &argv);
  gegl_init (&argc, &argv);

  if (argc != 2) {
    g_print ("Usage: %s <FILENAME>\n", argv[0]);
    exit(1);
  }

  /* Build graph that loads an image */
  graph = gegl_node_new ();
  node = gegl_node_new_child (graph,
    "operation", "gegl:load", 
    "path", argv[1], NULL);

  gegl_node_process (node);

  /* Setup */
  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (GTK_WINDOW (window), "GEGL-GTK transformation example");

  eventbox = gtk_event_box_new ();
  view = g_object_new (GEGL_GTK_TYPE_VIEW, "node", node, NULL);

  g_signal_connect (G_OBJECT (eventbox), "motion-notify-event",
                    (GCallback) motion_event, (gpointer)view);

  gtk_container_add (GTK_CONTAINER (eventbox), view);
  gtk_container_add (GTK_CONTAINER (window), eventbox);

  g_signal_connect (G_OBJECT (window), "delete-event",
                    G_CALLBACK (gtk_main_quit), window);
  gtk_widget_show_all (window);

  /* Run */
  gtk_main ();

  /* Cleanup */
  g_object_unref (graph);
  gtk_widget_destroy (window);
  gegl_exit ();
  return 0;
}
