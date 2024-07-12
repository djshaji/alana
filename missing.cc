#include "SharedLibrary.h"
#include "Plugin.h"
#include <filesystem>
#include <vector>
#include <iostream>
#include "json.hpp"
#include <lilv/lilv.h>

int main (int argc, char ** argv ) {
    std::string path = "libs";
    std::vector <std::string> aplugins ;
    for (const auto & entry : std::filesystem::directory_iterator(path)) {
        std::cout << entry.path() << std::endl;
        
        SharedLibrary s = SharedLibrary ((char *)entry.path ().u8string ().c_str (), SharedLibrary::PluginType::LADSPA) ;
        if (s.load () == NULL) {
            for (int i = 0 ; i < s.descriptors.size () ; i ++) {
                printf ("name: %s\n", s.descriptors.at (i)->Name);
                aplugins.push_back (s.descriptors.at (i)->Name);
            }
        }
        
        //~ for (int i = 0 ; i < plugins.size () ; i ++) {
            //~ printf ("plugin: %s\n", aplugins.at (i).c_str ());
        //~ }
        
        
    }
        
    std::ifstream fJson(argv [1]);
    std::stringstream buffer;
    buffer << fJson.rdbuf();
    auto json = nlohmann::json::parse(buffer.str());

    {
        LilvWorld* world = (LilvWorld* )lilv_world_new();
        lilv_world_load_all(world);
        LilvPlugins* plugins = (LilvPlugins* )lilv_world_get_all_plugins(world);
        LILV_FOREACH (plugins, i, plugins) {
            const LilvPlugin* p = (LilvPlugin* )lilv_plugins_get(plugins, i);
            const char * name = lilv_node_as_string (lilv_plugin_get_name (p));
            printf("[LV2] %s\n", name);
            aplugins.push_back (name);
            
        }
    }

    std::cout << "\n\n-------| missing |----------" << "\n\n" ;
    
    for (auto plugin : json) {
        if (! std::count (aplugins.begin (), aplugins.end (), plugin ["name"])) {
            std::cout << plugin ["name"]<< "\n";
        }
    }
    
    
}
