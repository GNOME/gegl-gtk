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

#include "config.h"

#include <math.h>
#include <babl/babl.h>
#include <glib-object.h>
#include <gtk/gtk.h>
#include <gegl.h>

#include "gegl-gtk-view.h"

enum
{
  PROP_0,
  PROP_NODE,
  PROP_X,
  PROP_Y,
  PROP_SCALE,
  PROP_BLOCK
};

enum
{
  SIGNAL_REDRAW,
  N_SIGNALS
};


typedef struct _GeglGtkViewPrivate
{
  GeglNode      *node;
  gint           x;
  gint           y;
  gdouble        scale;
  gboolean       block;    /* blocking render */

  guint          monitor_id;
  GeglProcessor *processor;
} GeglGtkViewPrivate;


G_DEFINE_TYPE (GeglGtkView, gegl_gtk_view, GTK_TYPE_DRAWING_AREA)
#define GEGL_GTK_VIEW_GET_PRIVATE(obj) \
  (G_TYPE_INSTANCE_GET_PRIVATE ((obj), GEGL_GTK_TYPE_VIEW, GeglGtkViewPrivate))

static guint gegl_gtk_view_signals[N_SIGNALS] = { 0 };

static void      gegl_gtk_view_class_init (GeglGtkViewClass  *klass);
static void      gegl_gtk_view_init       (GeglGtkView       *self);
static void      finalize             (GObject        *gobject);
static void      set_property         (GObject        *gobject,
                                       guint           prop_id,
                                       const GValue   *value,
                                       GParamSpec     *pspec);
static void      get_property         (GObject        *gobject,
                                       guint           prop_id,
                                       GValue         *value,
                                       GParamSpec     *pspec);
#ifdef HAVE_GTK2
static gboolean  expose_event         (GtkWidget      *widget,
                                       GdkEventExpose *event);
#endif
#ifdef HAVE_GTK3
static gboolean  draw                 (GtkWidget * widget,
                                       cairo_t *cr);
#endif

static void      redraw_event (GeglGtkView *view,
                               GeglRectangle *rect,
                               gpointer data);

static void      gegl_gtk_view_repaint       (GeglGtkView *view);

static void
gegl_gtk_view_class_init (GeglGtkViewClass * klass)
{
  GObjectClass   *gobject_class = G_OBJECT_CLASS (klass);
  GtkWidgetClass *widget_class  = GTK_WIDGET_CLASS (klass);

  gobject_class->finalize     = finalize;
  gobject_class->set_property = set_property;
  gobject_class->get_property = get_property;

#ifdef HAVE_GTK2
  widget_class->expose_event        = expose_event;
#endif

#ifdef HAVE_GTK3
  widget_class->draw                = draw;
#endif

  g_object_class_install_property (gobject_class, PROP_X,
                                   g_param_spec_int ("x",
                                                     "X",
                                                     "X origin",
                                                     G_MININT, G_MAXINT, 0,
                                                     G_PARAM_CONSTRUCT |
                                                     G_PARAM_READWRITE));
  g_object_class_install_property (gobject_class, PROP_Y,
                                   g_param_spec_int ("y",
                                                     "Y",
                                                     "Y origin",
                                                     G_MININT, G_MAXINT, 0,
                                                     G_PARAM_CONSTRUCT |
                                                     G_PARAM_READWRITE));
  g_object_class_install_property (gobject_class, PROP_SCALE,
                                   g_param_spec_double ("scale",
                                                        "Scale",
                                                        "Zoom factor",
                                                        0.0, 100.0, 1.00,
                                                        G_PARAM_CONSTRUCT |
                                                        G_PARAM_READWRITE));
  g_object_class_install_property (gobject_class, PROP_NODE,
                                   g_param_spec_object ("node",
                                                        "Node",
                                                        "The node to render",
                                                        G_TYPE_OBJECT,
                                                        G_PARAM_CONSTRUCT |
                                                        G_PARAM_READWRITE));
  g_object_class_install_property (gobject_class, PROP_BLOCK,
                                   g_param_spec_boolean ("block",
                                                        "Blocking render",
                                                        "Make sure all data requested to blit is generated.",
                                                        FALSE,
                                                        G_PARAM_READWRITE));

  /* Emitted when a redraw is needed, with the area that needs redrawing.
   * Exposed so that it can be tested. */
  gegl_gtk_view_signals[SIGNAL_REDRAW] = g_signal_new ("redraw",
                G_TYPE_FROM_CLASS (klass),
                G_SIGNAL_RUN_LAST | G_SIGNAL_ACTION,
                0,
                NULL, NULL,
                g_cclosure_marshal_VOID__BOXED,
                G_TYPE_NONE, 1,
                GEGL_TYPE_RECTANGLE);

   g_type_class_add_private (klass, sizeof (GeglGtkViewPrivate));
}

static void
gegl_gtk_view_init (GeglGtkView *self)
{
  GeglGtkViewPrivate *priv = GEGL_GTK_VIEW_GET_PRIVATE (self);
  priv->node        = NULL;
  priv->x           = 0;
  priv->y           = 0;
  priv->scale       = 1.0;
  priv->monitor_id  = 0;
  priv->processor   = NULL;

  g_signal_connect(self, "redraw", G_CALLBACK (redraw_event), NULL);
}

static void
finalize (GObject *gobject)
{
  GeglGtkView * self = GEGL_GTK_VIEW (gobject);
  GeglGtkViewPrivate *priv = GEGL_GTK_VIEW_GET_PRIVATE (self);

  if (priv->monitor_id)
    {  
      g_source_remove (priv->monitor_id);
      priv->monitor_id = 0;
    }

  if (priv->node)
    g_object_unref (priv->node);

  if (priv->processor)
    g_object_unref (priv->processor);

  G_OBJECT_CLASS (gegl_gtk_view_parent_class)->finalize (gobject);
}

static void
computed_event (GeglNode      *self,
                GeglRectangle *rect,
                GeglGtkView      *view)
{
  GeglGtkViewPrivate *priv = GEGL_GTK_VIEW_GET_PRIVATE (view);
  gint x = priv->scale * (rect->x) - priv->x;
  gint y = priv->scale * (rect->y) - priv->y;
  gint w = ceil (priv->scale * rect->width);
  gint h = ceil (priv->scale * rect->height);
  GeglRectangle redraw_rect = {x, y, w, h};

  g_signal_emit (view, gegl_gtk_view_signals[SIGNAL_REDRAW],
	         0, &redraw_rect, NULL);

}

static void
redraw_event (GeglGtkView *view,
              GeglRectangle *rect,
              gpointer data)
{
  gtk_widget_queue_draw_area (GTK_WIDGET (view),
			      rect->x, rect->y,
			      rect->width, rect->height);
}


static void
invalidated_event (GeglNode      *self,
                   GeglRectangle *rect,
                   GeglGtkView      *view)
{
  gegl_gtk_view_repaint (view);
}

static void
set_property (GObject      *gobject,
              guint         property_id,
              const GValue *value,
              GParamSpec   *pspec)
{
  GeglGtkView *self = GEGL_GTK_VIEW (gobject);
  GeglGtkViewPrivate *priv = GEGL_GTK_VIEW_GET_PRIVATE (self);

  switch (property_id)
    {
    case PROP_NODE:
      if (priv->node)
        {
          g_object_unref (priv->node);
        }

      if (g_value_get_object (value))
        {
          priv->node = GEGL_NODE (g_value_dup_object (value));

          g_signal_connect_object (priv->node, "computed",
                                   G_CALLBACK (computed_event),
                                   self, 0);
          g_signal_connect_object (priv->node, "invalidated",
                                   G_CALLBACK (invalidated_event),
                                   self, 0);
          gegl_gtk_view_repaint (self);
        }
      else

        {
          priv->node = NULL;
        }
      break;
    case PROP_X:
      priv->x = g_value_get_int (value);
      gtk_widget_queue_draw (GTK_WIDGET (self));
      break;
    case PROP_BLOCK:
      priv->block = g_value_get_boolean (value);
      break;
    case PROP_Y:
      priv->y = g_value_get_int (value);
      gtk_widget_queue_draw (GTK_WIDGET (self));
      break;
    case PROP_SCALE:
      priv->scale = g_value_get_double (value);
      gtk_widget_queue_draw (GTK_WIDGET (self));
      break;
    default:

      G_OBJECT_WARN_INVALID_PROPERTY_ID (gobject, property_id, pspec);
      break;
    }
}

static void
get_property (GObject      *gobject,
              guint         property_id,
              GValue       *value,
              GParamSpec   *pspec)
{
  GeglGtkView *self = GEGL_GTK_VIEW (gobject);
  GeglGtkViewPrivate *priv = GEGL_GTK_VIEW_GET_PRIVATE (self);

  switch (property_id)
    {
    case PROP_NODE:
      g_value_set_object (value, priv->node);
      break;
    case PROP_X:
      g_value_set_int (value, priv->x);
      break;
    case PROP_BLOCK:
      g_value_set_boolean (value, priv->block);
      break;
    case PROP_Y:
      g_value_set_int (value, priv->y);
      break;
    case PROP_SCALE:
      g_value_set_double (value, priv->scale);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (gobject, property_id, pspec);
      break;
    }
}

static void
draw_implementation (GeglGtkViewPrivate *priv, cairo_t *cr, GdkRectangle *rect)
{
  cairo_surface_t *surface = NULL;
  guchar          *buf = NULL;
  GeglRectangle   roi;

  roi.x = priv->x + rect->x;
  roi.y = priv->y + rect->y;
  roi.width  = rect->width;
  roi.height = rect->height;

  buf = g_malloc ((roi.width) * (roi.height) * 4);

  gegl_node_blit (priv->node,
                  priv->scale,
                  &roi,
                  babl_format ("B'aG'aR'aA u8"),
                  (gpointer)buf,
                  GEGL_AUTO_ROWSTRIDE,
                  GEGL_BLIT_CACHE | (priv->block ? 0 : GEGL_BLIT_DIRTY));

  surface = cairo_image_surface_create_for_data (buf, 
                                                 CAIRO_FORMAT_ARGB32, 
                                                 roi.width, roi.height, 
                                                 roi.width*4);
  cairo_set_source_surface (cr, surface, rect->x, rect->y);
  cairo_paint (cr);

  cairo_surface_finish (surface);
  g_free (buf);

}

#ifdef HAVE_GTK3
static gboolean
draw (GtkWidget * widget, cairo_t *cr)
{
  GeglGtkView      *view = GEGL_GTK_VIEW (widget);
  GeglGtkViewPrivate *priv = GEGL_GTK_VIEW_GET_PRIVATE (view);
  GdkRectangle rect;

  if (!priv->node)
    return FALSE;

  gdk_cairo_get_clip_rectangle (cr, &rect);

  draw_implementation (priv, cr, &rect);

  gegl_gtk_view_repaint (view);

  return FALSE;
}
#endif

#ifdef HAVE_GTK2
static gboolean
expose_event (GtkWidget      *widget,
              GdkEventExpose *event)
{
  GeglGtkView      *view = GEGL_GTK_VIEW (widget);
  GeglGtkViewPrivate *priv = GEGL_GTK_VIEW_GET_PRIVATE (view);
  cairo_t      *cr;
  GdkRectangle rect;

  if (!priv->node)
    return FALSE;

  cr = gdk_cairo_create (widget->window);
  gdk_cairo_region (cr, event->region);
  cairo_clip (cr);
  gdk_region_get_clipbox (event->region, &rect);

  draw_implementation (priv, cr, &rect);
  cairo_destroy (cr);

  gegl_gtk_view_repaint (view);

  return FALSE;
}
#endif

static gboolean
task_monitor (GeglGtkView *view)
{
  GeglGtkViewPrivate *priv = GEGL_GTK_VIEW_GET_PRIVATE (view);
  if (priv->processor==NULL)
    return FALSE;
  if (gegl_processor_work (priv->processor, NULL))
    return TRUE;

  priv->monitor_id = 0;

  return FALSE;
}

void
gegl_gtk_view_repaint (GeglGtkView *view)
{
  GtkWidget       *widget = GTK_WIDGET (view);
  GeglGtkViewPrivate *priv   = GEGL_GTK_VIEW_GET_PRIVATE (view);
  GeglRectangle    roi;
  GtkAllocation    allocation;

  roi.x = priv->x / priv->scale;
  roi.y = priv->y / priv->scale;
  gtk_widget_get_allocation (widget, &allocation);
  roi.width = ceil(allocation.width / priv->scale+1);
  roi.height = ceil(allocation.height / priv->scale+1);

  if (priv->monitor_id == 0)
    {
      priv->monitor_id = g_idle_add_full (G_PRIORITY_LOW,
                                          (GSourceFunc) task_monitor, view,
                                          NULL);

      if (priv->processor == NULL)
        {
          if (priv->node)
            priv->processor = gegl_node_new_processor (priv->node, &roi);
        }
    }

  if (priv->processor)
    gegl_processor_set_rectangle (priv->processor, &roi);
}
