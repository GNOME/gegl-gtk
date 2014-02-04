
class Examples.Basic : GLib.Object {

    public static int main(string[] args) {
        Gtk.init(ref args);
        Gegl.init(ref args);

        {
            var graph = new Gegl.Node();
            var node = graph.create_child("gegl:load");
            node.set_property("path", args[1]);

            var window = new Gtk.Window();
            window.title = "GEGL GTK Basic Vala example";
            window.set_default_size(300, 300);
            window.destroy.connect(Gtk.main_quit);

            var node_view = new GeglGtk.View();
            node_view.set_node(node);

            window.add(node_view);
            window.show_all();

            Gtk.main();
        }

        Gegl.exit();
        return 0;
    }
}

