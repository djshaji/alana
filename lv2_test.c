#include <lilv/lilv.h>

int main (int argc, char ** argv) {
    LilvWorld* world = (LilvWorld* )lilv_world_new();
    lilv_world_load_all(world);
    LilvPlugins* plugins = (LilvPlugins* )lilv_world_get_all_plugins(world);
    
    LilvNode* plugin_uri = lilv_new_uri(world, argv [1]);
    const LilvPlugin* plugin = lilv_plugins_get_by_uri(plugins, plugin_uri);
    const char * name = lilv_node_as_string (lilv_plugin_get_name (plugin));
    printf("[LV2] %s\n", name);

    //~ LILV_FOREACH (plugins, i, plugins) {
        //~ const LilvPlugin* p = (LilvPlugin* )lilv_plugins_get(plugins, i);
        //~ const char * name = lilv_node_as_string (lilv_plugin_get_name (p));
        //~ printf("[LV2] %s\n", name);
    //~ }
}
