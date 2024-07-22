#include "util.h"

json filename_to_json (std::string filename) {
    std::ifstream fJson(filename);
    std::stringstream buffer;
    buffer << fJson.rdbuf();
    auto j = json::parse(buffer.str ());
    return j ;
}

bool json_to_filename (json j, std::string filename) {
    LOGV (filename.c_str ());
    std::ofstream o(filename);
    o << std::setw(4) << j << std::endl;
    return true;
}
