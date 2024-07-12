#include "engine.h"

std::vector <Plugin *> *Engine::activePlugins = nullptr;

bool Engine::addPlugin(char* library, int pluginIndex, SharedLibrary::PluginType _type) {
    SharedLibrary * sharedLibrary = new SharedLibrary (library, _type);

    sharedLibrary ->setLibraryPath(std::string ("libs"));
    sharedLibrary->load();

    if (sharedLibrary->descriptors.size() == 0) {
        LOGE("Unable to load shared library!") ;
        return false;
    }

    LOGD("loaded shared library [ok] ... now trying to load plugin\n");
    Plugin * plugin = new Plugin (sharedLibrary->descriptors.at(pluginIndex), (long) sampleRate, _type);
    plugin->sharedLibrary = sharedLibrary;
    if (_type != SharedLibrary::LADSPA) {
        plugin -> load () ;
    }

    if (activePlugins == nullptr) {
        LOGD ("first run\n");
        activePlugins = new std::vector <Plugin *> () ;
    }
    activePlugins ->push_back(plugin);
    LOGD ("adding plugin to active chain %d\n", activePlugins->size ());
    // todo
//    buildPluginChain();

    return true ;
}
