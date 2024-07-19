#include "engine.h"

std::vector <Plugin *> *Engine::activePlugins = nullptr;

bool Engine::addPlugin(char* library, int pluginIndex, SharedLibrary::PluginType _type) {
    IN
    processor->bypass = true ;
    SharedLibrary * sharedLibrary = new SharedLibrary (library, _type);

    sharedLibrary ->setLibraryPath(std::string ("libs/linux/x86_64"));
    //~ sharedLibrary ->setLibraryPath(std::string ("libs/linux/x86_64"));
    sharedLibrary ->lv2_config_path = std::string ("assets/lv2");
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
    return driver->open ();
}

Engine::Engine () {
    libraryPath = strdup ("libs/linux/x86_64/");
    
    processor = new Processor () ;
    openAudio () ;
    
    ladspaPlugins  = new std::vector <std::string> ();
    lv2Plugins = new std::vector <std::string> ();
    
    std::ifstream fJson("lv2_plugins.json");
    std::stringstream buffer;
    buffer << fJson.rdbuf();

    lv2Json = nlohmann::json::parse(buffer.str());

    {
        std::ifstream fJson("all_plugins.json");
        std::stringstream buffer;
        buffer << fJson.rdbuf();
        ladspaJson = nlohmann::json::parse(buffer.str ());
    }
    
    for (auto plugin : ladspaJson) {
        std::string a = plugin ["name"].dump() ;
        ladspaPlugins->push_back (a.substr (1, a.size () - 2));
        //~ LOGD ("[ladspa] %s", a.c_str ());
    }

    for (auto plugin : lv2Json) {
        std::string a = plugin ["name"].dump() ;
        lv2Plugins->push_back (a.substr (1, a.size () - 2));
        //~ LOGD ("[lv2] %s", a.c_str ());
    }
    
    
    {
        std::ifstream fJson("assets/plugins.json");
        std::stringstream buffer;
        buffer << fJson.rdbuf();
        categories = nlohmann::json::parse(buffer.str ());
    }

    {
        std::ifstream fJson("assets/creator.json");
        std::stringstream buffer;
        buffer << fJson.rdbuf();
        creators = nlohmann::json::parse(buffer.str ());
    }

    initLilv ();
}

void Engine::initLilv () {
    IN
    LilvWorld* world = (LilvWorld* )lilv_world_new();
    lilv_world_load_all(world);
    plugins = (LilvPlugins* )lilv_world_get_all_plugins(world);        
    
    {
        std::ifstream fJson("assets/lv2_map.json");
        std::stringstream buffer;
        buffer << fJson.rdbuf();
        //~ std::cout << buffer.str () ;
        lv2Map = nlohmann::json::parse(buffer.str ());
    }
    
    OUT
}

std::vector <std::string> Engine::scanMissingLV2 () {
    std::vector <std::string> missing ;
    for (auto plugin : lv2Json) {
        std::string a = plugin ["name"].dump() ;
        bool found = false ;
        
        LILV_FOREACH (plugins, i, plugins) {
            const LilvPlugin* p = (LilvPlugin* )lilv_plugins_get(plugins, i);
            const char * name = lilv_node_as_string (lilv_plugin_get_name (p));
            
            if (strcmp (a.c_str (), name) == 0) {
                found = true ;
                break ;
            }
            
            std::string uri (lilv_node_as_string (lilv_plugin_get_uri (p)));
            LOGD ("[lilv] name no match, trying uri %s ... ", uri.c_str ());
            int c = uri.find ("#") ;
            if (c == -1)
                c = uri.find_last_of ("/");
            
            if (c != -1) {
                std::string stub = uri.substr (c + 1, uri.size () - 1) ;
                printf ("stub: %s ", stub.c_str ());
                if (strcmp (stub.c_str (), a.c_str ()) == 0) {
                    found = true ;
                    break ;
                }
            }
            
            LOGD (" give up\n");
        }

        if (! found) {
            missing.push_back (std::string (a.c_str ()));
            LOGD ("[missing] %s\n", a.c_str ());
        }
    }
    
    return missing ;
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
