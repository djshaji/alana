#ifndef UTIL_H
#define UTIL_H

#include <fstream>

#include "json.hpp"
#include "log.h"

using json = nlohmann::json;

json filename_to_json (std::string filename);

#endif
