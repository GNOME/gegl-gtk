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

gboolean
test_utils_display_is_set()
{
    return g_getenv("DISPLAY") != NULL;
}

void
test_utils_print_rect(GeglRectangle *rect)
{

    g_print("GeglRectangle: %d,%d %dx%d", rect->x, rect->y, rect->width, rect->height);
}

static gboolean
test_utils_quit_gtk_main(gpointer data)
{
    gtk_main_quit();
}

/* Compare two rectangles, output */
gboolean
test_utils_compare_rect(GeglRectangle *r, GeglRectangle *s)
{
    gboolean equal = gegl_rectangle_equal(r, s);
    if (!equal) {
        test_utils_print_rect(r);
        g_printf("%s", " != ");
        test_utils_print_rect(s);

    }
    return equal;
}
