#include "SharedLibrary.h"
#include "Plugin.h"
#include <filesystem>
#include <vector>
#include <iostream>
#include "json.hpp"

#ifdef __linux__
#include <lilv/lilv.h>
#endif 

void list_ladspa (std::string path) {
    for (const auto & entry : std::filesystem::directory_iterator(path)) {
        std::cout << entry.path() << std::endl;
        
        SharedLibrary s = SharedLibrary ((char *)entry.path ().u8string ().c_str (), SharedLibrary::PluginType::LADSPA) ;
        if (s.load () == NULL) {
            for (int i = 0 ; i < s.descriptors.size () ; i ++) {
                printf ("-----------------------------------\n");
                printf ("[%s] %d: %s\n", entry.path().c_str (), i, s.descriptors.at (i)->Name);
                printf ("-----------------------------------\n");
            }
        }        
    }
    
}

# ifdef __linux__
void list_lilv () {
    {
        LilvWorld* world = (LilvWorld* )lilv_world_new();
        lilv_world_load_all(world);
        LilvPlugins* plugins = (LilvPlugins* )lilv_world_get_all_plugins(world);
        LILV_FOREACH (plugins, i, plugins) {
            const LilvPlugin* p = (LilvPlugin* )lilv_plugins_get(plugins, i);
            const char * name = lilv_node_as_string (lilv_plugin_get_name (p));
            printf("[LV2] %s\n", name);            
        }
    }

    
}
# endif

int main (int argc, char ** argv ) {
    std::string path = "libs";
        
    std::ifstream fJson(argv [1]);
    std::stringstream buffer;
    buffer << fJson.rdbuf();
    auto json = nlohmann::json::parse(buffer.str());

    std::cout << "\n\n-------| missing |----------" << "\n\n" ;
    
    for (auto plugin : json) {
        path = plugin ["library"].dump () ;
        path = path.substr (1, path.size () - 2);
        std::string fpath = std::string ("libs/win32/") + path ;
        if (! std::filesystem::exists(fpath))
            printf ("[missing] %s\n", fpath.c_str ());
    }
    
    list_ladspa ("libs/win32");
    
    
}
