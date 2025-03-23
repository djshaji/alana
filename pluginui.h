#ifndef PLUGINUI_H
#define PLUGINUI_H

#include <gtk/gtk.h>
#include <vector>
#include <iostream>
#include <sstream>
#include <print>
#include <filesystem>

#include "knob.h"
#include "json.hpp"
#include "log.h"
#include "defs.h"
#include "engine.h"
#include "Plugin.h"
#include "PluginControl.h"
#include "callback_data.h"
#include "util.h"


using json = nlohmann::json;

void callback (void * p, void *c);
void bypass (void * p, bool, void * c)  ;
void control_changed (void * p, void * c);
void ui_file_chooser (void * b, void * d)  ;
void dropdown_activated (void * d, int, void * s)  ;
void pu_move_up (void * b, void * d)  ;
void pu_move_down (void * b, void * d) ;

void
control_port_set_real_val (Port * self, float val) ;

float
control_port_get_snapped_val (Port * self) ;

void knob_set (GtkSpinButton * w, float val) ;
float knob_get (GtkSpinButton * s) ;
typedef enum {
    FILE_AUDIO,
    FILE_JSON,
    FILE_ATOM
} PluginFileType ;

class PluginUI {
public:    
    int knob_sizes [4] = {48, 84, 100, 140};
    const char * bypassContains [10] = {
            "Pull the",
            "witch",
            "oggle",
            "prefilter",
            "bypass",
            "stick it!",
            "vibe",
            "hold",
            nullptr
    } ;

    GtkLabel * name ;
    GtkSwitch * onoff ;
    GtkButton * del, * up, * down ;
    Engine * engine ;
    void * rack ;
    Plugin * plugin ;
    GtkBox * card ;
    GtkWidget * card_ ;
    GtkBox * parent ;
    int index, * index_p ;
    PluginFileType * pType ;
    std::vector <GtkScale *> sliders ;
  
    void load_preset (std::string);
    int get_index ();
    GtkSpinButton * id ;
  
    PluginUI (Engine * _engine, Plugin * _plugin, GtkBox * _parent, std::string pluginName, int _index, bool has_file_chooser, void * _rack) ;
    void remove () ;
} ;

#include "rack.h"
#endif
