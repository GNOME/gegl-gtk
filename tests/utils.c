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
