#ifndef ENGINE_H
#define ENGINE_H

#include "SharedLibrary.h"
#include "Plugin.h"
#include "jack.h"
#include "process.h"

class Engine {
    int sampleRate ;
    AudioDriver * driver = nullptr;
    std::vector <SharedLibrary *> libraries ;
    Processor * processor = nullptr ;
public:
    nlohmann::json ladspaJson, lv2Json, creators, categories ;
    std::vector <std::string> * ladspaPlugins, * lv2Plugins ;

    Engine ();
    void buildPluginChain ();
    static std::vector<Plugin *> * activePlugins ;
    bool addPlugin(char* library, int pluginIndex, SharedLibrary::PluginType _type) ;
    bool openAudio ();
};

#endif 
