#ifndef UTIL_H
#define UTIL_H

#include <gtkmm.h>
#include <fstream>

#include "json.hpp"
#include "log.h"
#include "defs.h"

using json = nlohmann::json;

json filename_to_json (std::string filename);
bool json_to_filename (json j, std::string filename) ;
void alert_yesno (std::string title, std::string msg, GAsyncReadyCallback cb, gpointer data) ;

#endif
