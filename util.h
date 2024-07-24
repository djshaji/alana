#ifndef UTIL_H
#define UTIL_H

#include <gtkmm.h>
#include <fstream>
#include <iostream>

#include "json.hpp"
#include "log.h"
#include "defs.h"

using json = nlohmann::json;

typedef enum {
    ALERT_MESSAGE,
    ALERT_YESNO,
    ALERT_TEXT
} AlertType ;

class Alert_CB {
    public:
        GtkWidget * widget, * dialog , *widget2;
        gpointer data ;
};

json filename_to_json (std::string filename);
bool json_to_filename (json j, std::string filename) ;
void alert_yesno (std::string title, std::string msg, GAsyncReadyCallback cb, gpointer data) ;
void alert (char * title, char * msg, AlertType type, gpointer callback, gpointer data) ;

#endif
