#include "engine.h"

std::vector <Plugin *> *Engine::activePlugins = nullptr;

bool Engine::addPlugin(char* library, int pluginIndex, SharedLibrary::PluginType _type) {
    IN
    processor->bypass = true ;
    SharedLibrary * sharedLibrary = new SharedLibrary (library, _type);

    sharedLibrary ->setLibraryPath(std::string (libraryPath));
    //~ sharedLibrary ->setLibraryPath(std::string ("libs/linux/x86_64"));
    sharedLibrary ->lv2_config_path = std::string (assetPath).append ("/lv2");
    sharedLibrary->load();

    if (sharedLibrary->descriptors.size() == 0) {
        LOGE("Unable to load shared library!") ;
        processor->bypass = false ;
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

    buildPluginChain();
    processor->bypass = false ;
    OUT
    return true ;
}

bool Engine::openAudio () {
    driver = new AudioDriver (processor);
    bool val = driver->open ();
    if (val) {
        sampleRate = driver->get_sample_rate () ;
    } else 
        sampleRate = 48000 ; // sane default
    
    return val ;
}

Engine::Engine () {
    libraryPath = strdup ("libs/linux/x86_64/");
    if (! std::filesystem::exists (libraryPath)) {
        free(libraryPath);
        
        libraryPath = strdup ("/usr/share/amprack/libs/linux/x86_64/") ;        
    }
    
    if (std::filesystem::exists ("assets"))
        assetPath = std::string ("assets");
    else if (std::filesystem::exists ("/usr/share/amprack/assets"))
        assetPath = std::string ("/usr/share/amprack/assets");
    else  {
        LOGD ("CANNOT FIND ASSETS!\n");
        abort ();
    }
    
    if (! std::filesystem::exists (libraryPath)) {
        free(libraryPath);
        printf ("CANNOT FIND LIBRARIES!\n");
        abort () ;        
    }

    LOGD ("[engine] library path: %s\n", libraryPath);
    LOGD ("[engine] assets path: %s\n", assetPath.c_str ());

    processor = new Processor () ;
    openAudio () ;
    
    ladspaPlugins  = new std::vector <std::string> ();
    lv2Plugins = new std::vector <std::string> ();

    amps = filename_to_json (std::string (assetPath).append ("/amps.json"));
    lv2Json = filename_to_json (std::string (assetPath).append ("/lv2_plugins.json"));
    ladspaJson = filename_to_json (std::string (assetPath).append ("/all_plugins.json"));
    categories = filename_to_json (std::string (assetPath).append ("/plugins.json"));
    creators = filename_to_json (std::string (assetPath).append ("/creator.json"));
    knobs = filename_to_json (std::string (assetPath).append ("/knobs.json"));

    //~ initLilv ();
}


void Engine::buildPluginChain () {
    IN
    LOGD("building chain for %d plugins", activePlugins->size());
    
    processor->activePlugins = 0;
    for (int i = 0 ; i < activePlugins->size () ; i ++) {
        Plugin *p = activePlugins->at (i);
        
        if (!p->active)
            continue;
        processor->inputPorts [processor->activePlugins] = p->inputPort ;
        processor->outputPorts [processor->activePlugins] = p->outputPort ;
        processor->inputPorts2 [processor->activePlugins] = p->inputPort2 ;
        processor->outputPorts2 [processor->activePlugins] = p->outputPort2 ;
        if (p->type == SharedLibrary::LADSPA) {
            processor->connect_port [processor->activePlugins] = p->descriptor->connect_port ;
            processor->run [processor->activePlugins] = p->descriptor->run ;
            processor->run_adding [processor->activePlugins] = p->descriptor->run_adding ;
            processor->set_run_adding_gain [processor->activePlugins] = p->descriptor->set_run_adding_gain ;
            processor->descriptor [processor->activePlugins] = p->descriptor;
        } else /*if (p->type == SharedLibrary::LV2) */{
            processor->connect_port [processor->activePlugins] = reinterpret_cast<void (*)(
                    LADSPA_Handle, unsigned long, LADSPA_Data *)>(p->lv2Descriptor->connect_port);
            processor->run [processor->activePlugins] = reinterpret_cast<void (*)(
                    LADSPA_Handle, unsigned long)>(p->lv2Descriptor->run);
            processor->descriptor [processor->activePlugins] = reinterpret_cast<const LADSPA_Descriptor *>(p->lv2Descriptor);
        }

        processor->run_adding_gain [processor->activePlugins] = p->run_adding_gain ;
        processor->handle [processor->activePlugins] = p->handle ;
        processor->activePlugins ++ ;
    }

    OUT
}

bool Engine::addPluginByName (char * pluginName) {
    std::string stub = "";
    if (lv2Map .contains (pluginName)) {
        stub = lv2Map [pluginName].dump();
    } 

    LILV_FOREACH (plugins, i, plugins) {
        const LilvPlugin* p = (LilvPlugin* )lilv_plugins_get(plugins, i);
        const char * name = lilv_node_as_string (lilv_plugin_get_name (p));
        const char * uri = lilv_node_as_string (lilv_plugin_get_uri (p));
        
        if (strcmp (pluginName, name) == 0) {
            printf("[LV2] %s\n", name);        
            return addPlugin ((char *)uri, 0, SharedLibrary::PluginType::LILV);
        }

        std::string s (uri);
        int x = s.find ("#");
        if (x == -1)
            x = s.find_last_of ("/");
        
        s = s.substr (x + 1, s.size ());
        if (s == stub) {
            LOGD ("found mapped plugin %s -> %s\n", pluginName, stub);
            return addPlugin ((char *)uri, 0, SharedLibrary::PluginType::LILV);
        }
    }

    return false ;
}


json Engine::getPreset () {
    IN
    if (activePlugins == nullptr || activePlugins->size () == 0) {
        LOGD ("no active plugins\n");
        OUT
        return json {};

    }
    
    json preset = {} ;
    json plugins = {} ;
    
    for (int i = 0 ; i < activePlugins->size () ; i ++) {
        Plugin * plugin = activePlugins->at (i);
        json p = {};
        
        p ["name"] = plugin->lv2_name;
        
        std::string controls = std::string () ;
        for (int x = 0 ; x < plugin->pluginControls.size () ; x ++) {
            controls.append (std::to_string (*plugin->pluginControls.at (x)->def));
            if (x < (plugin->pluginControls.size () + 2))
                controls.append (";");
        }
        
        p ["controls"] = controls ;
        plugins [std::to_string (i)] = p ;
    }

    preset ["controls"] = plugins;
    OUT
    return preset ;
}

bool Engine::savePreset (std::string filename, std::string description) {
    IN
    LOGD ("[save preset] to file %s\n", filename.c_str ());
    json preset = getPreset () ;
    if (preset == NULL)
        return false ;
    preset ["desc"] = description ;
    preset ["name"] = std::string (filename).substr (filename.find_last_of ("/") + 1, filename.size ()) ;
    json_to_filename (preset, filename);
    LOGD ("[preset save] %s\n", preset.dump ().c_str ());
    OUT
    return true ;
}

bool Engine::load_preset (json j) {
    IN
    auto plugins = j ["controls"];
    for (auto p: plugins) {
        
    }
    OUT
    return true;
}

void Engine::set_plugin_audio_file (int index, char * filename) {
    IN
    SoundFile * sf = snd_read (filename) ;
    if (sf == NULL) {
        LOGD ("file read failed!\n");
        return ;
    } else {
        LOGD ("read %d bytes\n", * sf -> len);
    }
    
    LOGD ("file read ok! set plugin: %d [%d bytes]\n", index, * sf -> len);
    
    //~ for (int i = 0 ; i < * sf -> len; i ++)
        //~ LOGD ("[frame: %f]\n", sf -> data [i]);
    //~ activePlugins->at (index)->lv2Descriptor->connect_port(
        //~ activePlugins->at (index)->handle, 99, sf->len);
    //~ activePlugins->at (index)->lv2Descriptor->connect_port(
        //~ activePlugins->at (index)->handle, 100, sf->data);
    //~ *sf -> len = * sf -> len / 2 ;
    
    processor->bypass = true ;
    activePlugins->at (index)->setBuffer (sf ->data, * sf -> len);
    processor->bypass = false ;

    delete (sf) ;
    OUT
}

void Engine::set_plugin_file (int index, char * filename) {
    IN
    std::ifstream fJson(filename);
    std::stringstream buffer;
    buffer << fJson.rdbuf();
    int size = buffer.str ().size () ;
    processor->bypass = true ;
    //~ activePlugins->at (index)->setBuffer ((float *) buffer.str ().c_str (), size);
    activePlugins->at (index)->lv2Descriptor->connect_port(
        activePlugins->at (index)->handle, 99, & size);
    activePlugins->at (index)->lv2Descriptor->connect_port(
        activePlugins->at (index)->handle, 100, (void *)buffer.str().c_str ());
    processor->bypass = false ;
    printf ("%s\n", buffer.str().c_str ());
    OUT
}

int Engine :: moveActivePluginDown (int _p) {
    IN
    if (_p == activePlugins->size()) {
        OUT
        return _p ;
    }

    auto it = activePlugins->begin() + _p;
    std::rotate(it, it + 1, it + 2);
    buildPluginChain();
    OUT
    return _p + 1 ;
}

int Engine :: moveActivePluginUp (int _p) {
    IN
    if (_p == 0) {
        OUT
        return _p ;
    }


    processor->bypass = true ;
    
    LOGD ("[engine] move %d up\n", _p) ;

    auto it = activePlugins->begin() + _p;
    std::rotate(it - 1,  it, it + 1);
    buildPluginChain();
    
    processor->bypass = false ;
    
    OUT
    return _p - 1 ;
}

void Engine::print () {
    LOGD ("-------| rack |---------\n");
    for (int i = 0 ; i < activePlugins->size(); i ++) {
        LOGD ("[%d] %s\n", i, activePlugins->at (i)->lv2_name.c_str ());
    }
    
    LOGD ("------------------------\n");
}
