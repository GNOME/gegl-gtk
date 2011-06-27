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

gint
main (gint    argc,
      gchar **argv)
{
  GeglNode *graph = NULL;
  GeglNode *node = NULL;
  GeglNode *display = NULL;

  g_thread_init (NULL);
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
  display = gegl_node_new_child (graph,
    "operation", "gegl-gtk2:display", NULL);
  gegl_node_link_many (node, display, NULL);

  gegl_node_process (display);

  /* FIXME: operation must spin the gtk mainloop itself */
  while (gtk_events_pending ())
    gtk_main_iteration ();

  sleep (2);

  /* Cleanup */
  g_object_unref (graph);
  gegl_exit ();
  return 0;
}
