#ifndef ENGINE_H
#define ENGINE_H

#include "SharedLibrary.h"
#include "Plugin.h"

class Engine {
    int sampleRate ;
    std::vector <SharedLibrary *> libraries ;

public:
    static std::vector<Plugin *> * activePlugins ;
    bool addPlugin(char* library, int pluginIndex, SharedLibrary::PluginType _type) ;
};

#endif 
