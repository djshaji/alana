#include "rack.h"

void Rack::add () {
    engine -> addPlugin ("libs/dyson_compress_1403.so", 0, SharedLibrary::PluginType::LADSPA);
    //~ LOGD ("plugin controls: %d", engine -> activePlugins->at (0)->pluginControls.size ());
}
