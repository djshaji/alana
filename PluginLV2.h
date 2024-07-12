#ifndef PLUGIN_LV2
#define PLUGIN_LV2

std::string PluginLV2::getLV2JSON (std::string pluginName) {
    HERE LOGD ("plugin: %s\n", pluginName.c_str ());
    std::ifstream fJson("assets/lv2_plugins.json");
    std::stringstream buffer;
    buffer << fJson.rdbuf();
    auto json = nlohmann::json::parse(buffer.str());
    
    for (auto plugin : json) {
        if (std::string (plugin ["name"]) == pluginName) {
            
        }
    }
    
    return nullptr;
}

#endif
