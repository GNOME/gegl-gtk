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
 * Copyright (C) 2006 Øyvind Kolås <pippin@gimp.org>
 * Copyright (C) 2011 Jon Nordby <jononor@gmail.com>
 */

#include "config.h"
#include <glib/gi18n-lib.h>


#ifdef GEGL_CHANT_PROPERTIES

gegl_chant_string(window_title, _("Window Title"), "",
                  _("Title to give window, if no title given inherits name of the pad providing input."))

#else

#define GEGL_CHANT_TYPE_SINK
#define GEGL_CHANT_C_FILE       "gegl-gtk-display.c"

#include <gegl.h>
#include <gegl-chant.h>

#include <gtk/gtk.h>
#include <gegl-gtk.h>

typedef struct {
    GtkWidget *window;
    GtkWidget *view_widget;
    GeglNode  *node;
    GeglNode  *input;
    gint       width;
    gint       height;
} Priv;

static Priv *
init_priv(GeglOperation *operation)
{
    GeglChantO *o = GEGL_CHANT_PROPERTIES(operation);

    if (!o->chant_data) {
        Priv *priv = g_new0(Priv, 1);
        o->chant_data = (void *) priv;

        gtk_init(0, 0);

        priv->window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
        priv->view_widget = g_object_new(GEGL_GTK_TYPE_VIEW, NULL);
        gtk_container_add(GTK_CONTAINER(priv->window), priv->view_widget);
        priv->width = -1;
        priv->height = -1;
        gtk_widget_set_size_request(priv->view_widget, priv->width, priv->height);
        gtk_window_set_title(GTK_WINDOW(priv->window), o->window_title);

        priv->node = NULL;
        priv->input = NULL;

        gtk_widget_show_all(priv->window);
    }
    return (Priv *)(o->chant_data);
}

static void
set_window_attributes(GeglOperation *operation, const GeglRectangle *result)
{
    GeglChantO *o = GEGL_CHANT_PROPERTIES(operation);
    Priv       *priv = init_priv(operation);

    if (priv->width != result->width  ||
            priv->height != result->height) {
        priv->width = result->width ;
        priv->height = result->height;
        gtk_widget_set_size_request(priv->view_widget, priv->width, priv->height);
    }


    if (priv->window) {
        gtk_window_resize(GTK_WINDOW(priv->window), priv->width, priv->height);
        if (o->window_title && o->window_title[0] != '\0') {
            gtk_window_set_title(GTK_WINDOW(priv->window), o->window_title);
        } else {
            const gchar *gegl_node_get_debug_name(GeglNode * node);
            gtk_window_set_title(GTK_WINDOW(priv->window),
                                 gegl_node_get_debug_name(gegl_node_get_producer(operation->node, "input", NULL))
                                );
        }
    }
}

/* Create an input proxy, and initial display operation, and link together.
 * These will be passed control when process is called later. */
static void
attach(GeglOperation *operation)
{
    Priv       *priv = init_priv(operation);

    g_assert(!priv->input);
    g_assert(!priv->node);

    priv->input = gegl_node_get_input_proxy(operation->node, "input");
    priv->node = gegl_node_new_child(operation->node,
                                     "operation", "gegl:nop",
                                     NULL);

    gegl_node_link(priv->input, priv->node);
    g_object_set(G_OBJECT(priv->view_widget), "node", priv->node, NULL);
}

static void
dispose(GObject *object)
{
    GeglChantO *o = GEGL_CHANT_PROPERTIES(object);
    Priv       *priv = (Priv *)o->chant_data;

    if (priv) {
        gtk_widget_destroy(priv->window);
        g_free(priv);
        o->chant_data = NULL;
    }

    G_OBJECT_CLASS(g_type_class_peek_parent(G_OBJECT_GET_CLASS(object)))->dispose(object);
}


static void
gegl_chant_class_init(GeglChantClass *klass)
{
    GeglOperationClass     *operation_class = GEGL_OPERATION_CLASS(klass);
    GeglOperationSinkClass *sink_class = GEGL_OPERATION_SINK_CLASS(klass);

    operation_class->attach = attach;
    G_OBJECT_CLASS(klass)->dispose = dispose;

#ifdef HAVE_GTK2
    operation_class->name        = "gegl-gtk2:display";
#else
    operation_class->name        = "gegl-gtk3:display";
#endif
    operation_class->categories  = "output";
    operation_class->description =
        _("Displays the input buffer in an GTK window .");
}

#endif
