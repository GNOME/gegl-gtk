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
#include <stdlib.h>
#include <glib.h>
#include <gegl.h>
#include <gtk/gtk.h>
#include <gegl-gtk.h>

gint
main(gint    argc,
     gchar **argv)
{
    GtkWidget *window = NULL;
    GtkWidget *view = NULL;
    GeglNode *graph = NULL;
    GeglNode *node = NULL;

    gtk_init(&argc, &argv);
    gegl_init(&argc, &argv);

    if (argc != 2) {
        g_print("Usage: %s <FILENAME>\n", argv[0]);
        exit(1);
    }

    /* Build graph that loads an image */
    graph = gegl_node_new();
    node = gegl_node_new_child(graph,
                               "operation", "gegl:load",
                               "path", argv[1], NULL);

    gegl_node_process(node);

    /* Setup */
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "GEGL-GTK basic example");

    view = GTK_WIDGET(gegl_gtk_view_new_for_node(node));
    gtk_container_add(GTK_CONTAINER(window), view);

    g_signal_connect(window, "destroy",
                     G_CALLBACK(gtk_main_quit), NULL);
    gtk_widget_show_all(window);

    /* Run */
    gtk_main();

    /* Cleanup */
    g_object_unref(graph);
    gegl_exit();
    return 0;
}
