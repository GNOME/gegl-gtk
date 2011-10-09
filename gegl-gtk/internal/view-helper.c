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
 * Copyright (C) 2003, 2004, 2006 Øyvind Kolås <pippin@gimp.org>
 */

#include "view-helper.h"

#include <math.h>
#include <babl/babl.h>


G_DEFINE_TYPE (ViewHelper, view_helper, G_TYPE_OBJECT)


enum
{
  SIGNAL_REDRAW_NEEDED,
  SIGNAL_SIZE_CHANGED,
  N_SIGNALS
};

static guint view_helper_signals[N_SIGNALS] = { 0 };

static void
finalize (GObject *gobject);

static void
view_helper_class_init (ViewHelperClass * klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

  gobject_class->finalize = finalize;

  /* Emitted when a redraw is needed, with the area that needs redrawing. */
  view_helper_signals[SIGNAL_REDRAW_NEEDED] = g_signal_new ("redraw-needed",
                G_TYPE_FROM_CLASS (klass),
                G_SIGNAL_RUN_LAST | G_SIGNAL_ACTION,
                0,
                NULL, NULL,
                g_cclosure_marshal_VOID__BOXED,
                G_TYPE_NONE, 1,
                GEGL_TYPE_RECTANGLE);

  /* Emitted when the size of the view changes, with the new size. */
  view_helper_signals[SIGNAL_SIZE_CHANGED] = g_signal_new ("size-changed",
                G_TYPE_FROM_CLASS (klass),
                G_SIGNAL_RUN_LAST | G_SIGNAL_ACTION,
                0,
                NULL, NULL,
                g_cclosure_marshal_VOID__BOXED,
                G_TYPE_NONE, 1,
                GEGL_TYPE_RECTANGLE);
}

static void
view_helper_init (ViewHelper *self)
{
  GeglRectangle invalid_rect = {0, 0, -1, -1};
  GdkRectangle invalid_gdkrect = {0, 0, -1, -1};

  self->node        = NULL;
  self->x           = 0;
  self->y           = 0;
  self->scale       = 1.0;
  self->monitor_id  = 0;
  self->processor   = NULL;

  self->widget_allocation = invalid_gdkrect;
  self->view_bbox   = invalid_rect;
}

static void
finalize (GObject *gobject)
{
   ViewHelper *self = VIEW_HELPER(gobject);

  if (self->monitor_id)
    {
      g_source_remove (self->monitor_id);
      self->monitor_id = 0;
    }

  if (self->node)
    g_object_unref (self->node);

  if (self->processor)
    g_object_unref (self->processor);
}

/* Transform a rectangle from model to view coordinates. */
static void
model_rect_to_view_rect(ViewHelper *self, GeglRectangle *rect)
{
  GeglRectangle temp;

  temp.x = self->scale * (rect->x) - rect->x;
  temp.y = self->scale * (rect->y) - rect->y;
  temp.width = ceil (self->scale * rect->width);
  temp.height = ceil (self->scale * rect->height);

  *rect = temp;
}

static void
invalidated_event (GeglNode      *node,
                   GeglRectangle *rect,
                   ViewHelper    *self)
{
  view_helper_repaint (self);
}

static gboolean
task_monitor (ViewHelper *self)
{
  if (self->processor==NULL)
    return FALSE;
  if (gegl_processor_work (self->processor, NULL))
    return TRUE;

  self->monitor_id = 0;

  return FALSE;
}


/* When the GeglNode has been computed,
 * find out if the size of the vie changed and
 * emit the "size-changed" signal to notify view
 * find out which area in the view was computed and emit the
 * "redraw-needed" signal to notify it that a redraw is needed */
static void
computed_event (GeglNode      *node,
                GeglRectangle *rect,
                ViewHelper    *self)
{
  /* Notify about potential size change */
  GeglRectangle bbox = gegl_node_get_bounding_box(node);
  model_rect_to_view_rect(self, &bbox);

  if (!gegl_rectangle_equal(&bbox, &(self->view_bbox))) {
      self->view_bbox = bbox;
      g_signal_emit (self, view_helper_signals[SIGNAL_SIZE_CHANGED],
                 0, &bbox, NULL);
  }

  /* Emit redraw-needed */
  GeglRectangle redraw_rect = *rect;
  model_rect_to_view_rect(self, &redraw_rect);

  g_signal_emit (self, view_helper_signals[SIGNAL_REDRAW_NEEDED],
	         0, &redraw_rect, NULL);
}

ViewHelper *
view_helper_new(void)
{
    return VIEW_HELPER(g_object_new(VIEW_HELPER_TYPE, NULL));
}

/* Draw the view of the GeglNode to the provided cairo context,
 * taking into account transformations et.c.
 * @rect the bounding box of the area to draw in view coordinates
 *
 * For instance called by widget during the draw/expose */
void
view_helper_draw (ViewHelper *self, cairo_t *cr, GdkRectangle *rect)
{
  cairo_surface_t *surface = NULL;
  guchar          *buf = NULL;
  GeglRectangle   roi;

  roi.x = self->x + rect->x;
  roi.y = self->y + rect->y;
  roi.width  = rect->width;
  roi.height = rect->height;

  buf = g_malloc ((roi.width) * (roi.height) * 4);

  gegl_node_blit (self->node,
                  self->scale,
                  &roi,
                  babl_format ("B'aG'aR'aA u8"),
                  (gpointer)buf,
                  GEGL_AUTO_ROWSTRIDE,
                  GEGL_BLIT_CACHE | (self->block ? 0 : GEGL_BLIT_DIRTY));

  surface = cairo_image_surface_create_for_data (buf,
                                                 CAIRO_FORMAT_ARGB32,
                                                 roi.width, roi.height,
                                                 roi.width*4);
  cairo_set_source_surface (cr, surface, rect->x, rect->y);
  cairo_paint (cr);

  cairo_surface_destroy (surface);
  g_free (buf);

}

void
view_helper_set_allocation(ViewHelper *self, GdkRectangle *allocation)
{
    self->widget_allocation = *allocation;
    view_helper_repaint(self);
}

/* Trigger processing of the GeglNode */
void
view_helper_repaint (ViewHelper *self)
{
  GeglRectangle    roi;

  roi.x = self->x / self->scale;
  roi.y = self->y / self->scale;

  roi.width = ceil(self->widget_allocation.width / self->scale+1);
  roi.height = ceil(self->widget_allocation.height / self->scale+1);

  if (self->monitor_id == 0)
    {
      self->monitor_id = g_idle_add_full (G_PRIORITY_LOW,
                                          (GSourceFunc) task_monitor, self,
                                          NULL);

      if (self->processor == NULL)
        {
          if (self->node)
            self->processor = gegl_node_new_processor (self->node, &roi);
        }
    }

  if (self->processor)
    gegl_processor_set_rectangle (self->processor, &roi);
}

void
invalidate(ViewHelper *self)
{
    GeglRectangle redraw_rect = {0, 0, -1, -1}; /* Indicates full redraw */
    g_signal_emit (self, view_helper_signals[SIGNAL_REDRAW_NEEDED],
            0, &redraw_rect, NULL);
}

void
view_helper_set_node(ViewHelper *self, GeglNode *node)
{
    if (self->node == node)
        return;

    if (self->node)
        g_object_unref (self->node);

    if (node) {
        g_object_ref (node);
        self->node = node;

        g_signal_connect_object (self->node, "computed",
                               G_CALLBACK (computed_event),
                               self, 0);
        g_signal_connect_object (self->node, "invalidated",
                               G_CALLBACK (invalidated_event),
                               self, 0);

        view_helper_repaint (self);

    } else
        self->node = NULL;
}

GeglNode *
view_helper_get_node(ViewHelper *self)
{
    return self->node;
}

void
view_helper_set_scale(ViewHelper *self, float scale)
{
    if (self->scale == scale)
        return;

    self->scale = scale;
    invalidate(self);
}

float
view_helper_get_scale(ViewHelper *self)
{
    return self->scale;
}

void
view_helper_set_x(ViewHelper *self, float x)
{
    if (self->x == x)
        return;

    self->x = x;
    invalidate(self);
}

float
view_helper_get_x(ViewHelper *self)
{
    return self->x;
}

void
view_helper_set_y(ViewHelper *self, float y)
{
    if (self->y == y)
        return;

    self->y = y;
    invalidate(self);
}

float
view_helper_get_y(ViewHelper *self)
{
    return self->y;
}
