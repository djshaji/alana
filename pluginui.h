#ifndef PLUGINUI_H
#define PLUGINUI_H

#include <gtkmm.h>
#include <gtk/gtk.h>
#include <vector>
#include <iostream>
#include <sstream>

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
    FILE_JSON
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

    Gtk::Label name ;
    Gtk::Switch  onoff ;
    Gtk::Button  del,  up,  down ;
    Engine * engine ;
    void * rack ;
    Plugin * plugin ;
    Gtk::Box  card ;
    GtkWidget * card_ ;
    Gtk::Box * parent ;
    int index, * index_p ;
    PluginFileType * pType ;
    std::vector <GtkScale *> sliders ;
  
    void load_preset (std::string);
    int get_index ();
    Gtk::SpinButton id ;
  
    PluginUI (Engine * _engine, Plugin * _plugin, Gtk::Box * _parent, std::string pluginName, int _index, bool has_file_chooser, void * _rack) ;
    void remove () ;
} ;

#include "rack.h"
#endif
