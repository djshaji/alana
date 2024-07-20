#include "util.h"

json filename_to_json (std::string filename) {
    std::ifstream fJson(filename);
    std::stringstream buffer;
    buffer << fJson.rdbuf();
    auto j = json::parse(buffer.str ());
    return j ;
}
