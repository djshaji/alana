#ifndef ENGINE_H
#define ENGINE_H

#include <lilv/lilv.h>
#include <iostream>
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
    char * libraryPath = nullptr; //= std::string ("libs/linux/x86_64/");
    nlohmann::json ladspaJson, lv2Json, creators, categories, lv2Map ;
    std::vector <std::string> * ladspaPlugins, * lv2Plugins ;
    LilvPlugins* plugins = nullptr ;
    
    Engine ();
    void buildPluginChain ();
    static std::vector<Plugin *> * activePlugins ;
    bool addPlugin(char* library, int pluginIndex, SharedLibrary::PluginType _type) ;
    bool openAudio ();
    bool addPluginByName (char *);
    void initLilv () ;
    std::vector <std::string> scanMissingLV2 ();
};

#endif 
