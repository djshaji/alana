#ifndef RACK_H
#define RACK_H

#include <gtkmm.h>
#include <gtk/gtk.h>
#include <vector>
#include <iostream>
#include <map>

#include "json.hpp"
#include "log.h"
#include "defs.h"
#include "engine.h"
#include "Plugin.h"
#include "PluginControl.h"
#include "pluginui.h"
#include "util.h"

typedef struct _Sorter {
    GtkWidget * creators, * categories, * listBox;
    Engine * engine ;
    std::vector <GtkWidget *> * boxes;
    void * rack ;
} Sorter;

class Rack {
public:
    Engine * engine ;
    std::map <int, GtkWidget*> pMap ;
    Gtk::Box master, mixer;
    Gtk::HeaderBar button_box ;
    Gtk::Box list_box ;
    Gtk::Overlay overlay ;
    Gtk::Button add_effect ;
    Gtk::ScrolledWindow sw ;
    std::vector <GtkWidget *> plugs;
    
    void move_up (int);
    void move_down (int);

    Gtk::Button logo, menu_button ;
    Gtk::ToggleButton mixer_toggle, record ;
    GtkWidget * listBox ;
    Gtk::Switch onoff ;
    
    std::vector <PluginUI> plugins ;        
    std::vector <GtkWidget *> hearts ;
    void add ();
    PluginUI * addPluginByName (char *);
    bool load_preset (json);
    bool load_preset (std::string filename);
    
    GtkWidget * pluginDialog, * rack ;
    GtkWidget * createPluginDialog () ;
    GtkWidget * addPluginEntry (std::string plug) ;
    
    json blacklist ;
    GtkWidget * search ;
    
    void clear () ;
    Rack () ;
};

#endif // RACK_H
