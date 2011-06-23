/* This file is part of GEGL editor -- a gtk frontend for GEGL
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
 * Copyright (C) 2003, 2004, 2006 Øyvind Kolås
 */

#ifndef __GEGL_GTK_VIEW_H__
#define __GEGL_GTK_VIEW_H__

G_BEGIN_DECLS

#define GEGL_GTK_TYPE_VIEW            (gegl_gtk_view_get_type ())
#define GEGL_GTK_VIEW(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), GEGL_GTK_TYPE_VIEW, GeglGtkView))
#define GEGL_GTK_VIEW_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass),  GEGL_GTK_TYPE_VIEW, GeglGtkViewClass))
#define GEGL_GTK_IS_VIEW(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GEGL_GTK_TYPE_VIEW))
#define GEGL_GTK_IS_VIEW_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),  GEGL_GTK_TYPE_VIEW))
#define GEGL_GTK_VIEW_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),  GEGL_GTK_TYPE_VIEW, GeglGtkViewClass))

typedef struct _GeglGtkView        GeglGtkView;
typedef struct _GeglGtkViewClass   GeglGtkViewClass;

struct _GeglGtkView
{
  GtkDrawingArea parent_instance;

};

struct _GeglGtkViewClass
{
  GtkDrawingAreaClass parent_class;
};

GType           gegl_gtk_view_get_type      (void) G_GNUC_CONST;

G_END_DECLS

#endif /* __GEGL_GTK_VIEW_H__ */
