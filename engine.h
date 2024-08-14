#ifndef ENGINE_H
#define ENGINE_H

#include <lilv/lilv.h>
#include <iostream>
#include <filesystem>
#include <gtkmm.h>
#include "SharedLibrary.h"
#include "Plugin.h"
#include "jack.h"
#include "process.h"
#include "util.h"
#include "snd.h"

using json = nlohmann::json;

class Engine {
public:
    int sampleRate ;
    AudioDriver * driver = nullptr;
    std::vector <SharedLibrary *> libraries ;
    Processor * processor = nullptr ;
    char * libraryPath = nullptr; //= std::string ("libs/linux/x86_64/");
    std::string assetPath = std::string ();
    nlohmann::json ladspaJson, lv2Json, creators, categories, lv2Map, amps, knobs ;
    std::vector <std::string> * ladspaPlugins, * lv2Plugins ;
    LilvPlugins* plugins = nullptr ;
    
    Engine ();
    void buildPluginChain ();
    int moveActivePluginDown (int);
    int moveActivePluginUp (int);
    
    static std::vector<Plugin *> * activePlugins ;
    bool addPlugin(char* library, int pluginIndex, SharedLibrary::PluginType _type) ;
    bool openAudio ();
    bool addPluginByName (char *);
    bool savePreset (std::string, std::string);
    bool load_preset (json );
    json getPreset ();
    void set_plugin_audio_file (int index, char * filename);
    void set_plugin_file (int index, char * filename) ;
    void print ();
};

#endif 
