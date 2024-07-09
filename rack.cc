#include "rack.h"

void Rack::add () {
    engine -> addPlugin ("libs/dyson_compress_1403.so", 0, SharedLibrary::PluginType::LADSPA);

    int index = engine -> activePlugins->size () - 1;
    PluginUI ui = PluginUI (engine, engine -> activePlugins->at (index), std::string ("Dyson Compressor"));
    // ui.index = index ;
    list_box.set_orientation (Gtk::Orientation::VERTICAL);
    list_box.set_vexpand (true);
    list_box.append (ui.card);
}
