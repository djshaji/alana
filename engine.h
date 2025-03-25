#ifndef ENGINE_H
#define ENGINE_H

#ifdef __linux__
//~ #include <lilv/lilv.h>
#include <sys/utsname.h>
#include "snd.h"
#include "jack.h"
#else
#include "pa.h"
#endif 

#include <iostream>
#include <filesystem>

#include <unistd.h>

#include "SharedLibrary.h"
#include "Plugin.h"
#include "process.h"
#include "util.h"
#include "LockFreeQueue.h"
#include "FileWriter.h"

using json = nlohmann::json;

class Engine {
public:
    int sampleRate ;
    FileWriter * fileWriter ;
    AudioDriver * driver = nullptr;
    std::string home ;
    std::vector <SharedLibrary *> libraries ;
    Processor * processor = nullptr ;
    char * libraryPath = nullptr; //= std::string ("libs/linux/x86_64/");
    std::string assetPath = std::string ();
    nlohmann::json ladspaJson, lv2Json, creators, categories, lv2Map, amps, knobs ;
    std::vector <std::string> * ladspaPlugins, * lv2Plugins ;
    LilvPlugins* plugins = nullptr ;
    LockFreeQueueManager * queueManager ;
    
    Engine ();
    void buildPluginChain ();
    int moveActivePluginDown (int);
    int moveActivePluginUp (int);
    void set_atom_port (int index, int control, char * filename);
    
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
    void startRecording ();
    void stopRecording ();
    static int check_notify (AudioBuffer * a) ;
};

#endif 
