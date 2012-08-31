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
 * Copyright (C) 2012 Jon Nordby <jononor@gmail.com>
 */

/* References
	http://library.gnome.org/devel/gdk-pixbuf/unstable/GdkPixbufLoader.html
	http://library.gnome.org/devel/gdk-pixbuf/stable/gdk-pixbuf-Module-Interface.html
*/

#define GDK_PIXBUF_ENABLE_BACKEND

#include <glib.h>
#include <gmodule.h>
#include <gdk-pixbuf/gdk-pixbuf.h>

#include <gegl.h>

static GQuark error_domain;


/* FIXME: use gio instead */
int write_to_temporary_file(FILE *infile, char* filename, GError **error) {
	FILE *outfile =	fopen(filename, "w");
	if (!outfile) {
		*error = g_error_new(error_domain, 1, "Could not open temporary file.");
/*		close(filename);*/
/*		return -1;*/
	}
	int i=0;

	while (1) {
		unsigned char c = fgetc(infile);
		if (c == EOF) {
			break;
		}
		if (fputc(c, outfile) == EOF) {
			*error = g_error_new(error_domain, 2, "Error during write to temporary file.");
			close(filename);
			return -1;
		}
		i++;
/*		if (i % 100000) printf("100K");*/
		if (i == 0) printf("overflow");
	}

	close(filename);
	return 0;
}

/* Load a fully rendered OpenRaster image */
static GdkPixbuf*
ora_image_load (FILE *f, GError **error) {

#if 0
	GdkPixbuf *pixbuf = NULL;
	GdkColorspace colorspace = GDK_COLORSPACE_RGB;
	gboolean has_alpha = TRUE;
	int bits_per_sample = 8;
	guchar *data = NULL;
	int width = -1;
	int height = -1;
	int rowstride = -1;
	ORA ora = NULL;
	int retval = ORA_OK;

	error_domain = g_quark_from_string("io-ora");

	/* HACKY: write file to a temporary, on-disk file, then pass that filename
	 * to libora. This is done because libora does not provide a way to 
	 * read the document from a FILE or memory buffer. */
	char tmp_filename[L_tmpnam];
	tmpnam(tmp_filename);

	printf("%s", tmp_filename);

	retval = write_to_temporary_file(f, tmp_filename, error);
	if (retval) {
		// remove(tmp_filename);
		return NULL;
	}

	retval = ora_open(tmp_filename, ORA_FILE_READ, &ora);
	if (!ora) {
		*error = g_error_new(error_domain, retval, "libora could not open the OpenRaster document");
		remove(tmp_filename);
		return NULL;
	}

	ora_get_document_size(ora, &width, &height);
	ora_render_document(ora, &data);
	ora_close(ora);
	remove(tmp_filename);

	/* FIXME: attach destroy function to free data */
	rowstride = width*4;
	pixbuf = gdk_pixbuf_new_from_data(data, colorspace, has_alpha, bits_per_sample,
										width, height, rowstride,
										NULL, NULL);

	return pixbuf;
	
#endif
    return NULL;
}

/* Register plugin */
#define MODULE_ENTRY(function) G_MODULE_EXPORT void function

MODULE_ENTRY (fill_vtable) (GdkPixbufModule *module)
{
        module->load = ora_image_load;
}

MODULE_ENTRY (fill_info) (GdkPixbufFormat *info)
{
	/* FIXME: set correctly
	OpenRaster files are ZIP archives, but have a file called mimetype STORED
	in the beginning of the archive. So "mimetypeimage/openraster" should exist,
	starting on the 18th byte
	{ "mimetypeimage/openraster", "", 100 }, */

    static GdkPixbufModulePattern signature[] = {
		{ "PK", NULL, 80 },
		{ NULL, NULL, 0 }
    };
	static gchar * mime_types[] = {
		"image/openraster",
		NULL
	};
	static gchar * extensions[] = {
		"ora",
		NULL
	};

	info->name = "ora";
    info->signature = signature;
	info->description = "OpenRaster";
	info->mime_types = mime_types;
	info->extensions = extensions;
	info->flags = 0;
	info->license = "LGPL";
}

