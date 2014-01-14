#!/usr/bin/env python
import sys, os.path

import gi
from gi.repository import Gegl, GeglGtk3, Gtk

graph_xml = """
<gegl>
  <gegl:crop width='512' height='512'/>
  <gegl:over >
    <gegl:translate x='30' y='30'/>
    <gegl:dropshadow radius='1.5' x='3' y='3'/>
    <gegl:text size='80' color='white'><params>
      <param name='string'>GEGL GTK</param>
      </params>
    </gegl:text>
  </gegl:over>
  <gegl:unsharp-mask std-dev='30'/>
  <gegl:load path='%s'/>
</gegl>"""

if __name__ == '__main__':
    if len(sys.argv) != 2:
        print 'Usage: %s FILE' % sys.argv[0]
        sys.exit(1)

    file_path = os.path.abspath(sys.argv[1])

    Gegl.init([])
    node = Gegl.Node.new_from_xml(graph_xml % file_path, "/");
    view_widget = GeglGtk3.View()
    view_widget.set_property('node', node.get_children()[-1])

    window = Gtk.Window()
    window.connect("destroy", Gtk.main_quit)
    window.add(view_widget)
    window.show_all()
    Gtk.main()
