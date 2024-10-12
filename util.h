#ifndef UTIL_H
#define UTIL_H

#include <fstream>
#include <iostream>
#include <filesystem>
#include <gtk/gtk.h>

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
void msg (std::string message) ;
bool download_file (char *name, const char * filename) ;
void copy_file (std::string src, std::string dest) ;
char ** list_directory (std::string dir) ;
void set_random_background (GtkWidget * widget) ;


#endif
