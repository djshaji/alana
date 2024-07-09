#include "rack.h"

void Rack::add () {
    engine -> addPlugin ("libs/dyson_compress_1403.so", 0, SharedLibrary::PluginType::LADSPA);

    PluginUI ui = PluginUI (engine, engine -> activePlugins->at (0), std::string ("Dyson Compressor"));
    list_box.set_orientation (Gtk::Orientation::VERTICAL);
    list_box.set_vexpand (true);
    list_box.append (ui.card);
}
