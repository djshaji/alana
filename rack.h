#ifndef RACK_H
#define RACK_H

//~ #include <gtkmm.h>
#include <gtk/gtk.h>
#include <vector>
#include <iostream>
#include <map>

#include "version.h"
#include "json.hpp"
#include "log.h"
#include "defs.h"
#include "engine.h"
#include "Plugin.h"
#include "PluginControl.h"
#include "pluginui.h"
#include "util.h"

using json = nlohmann::json;

typedef struct _Sorter {
    GtkWidget * creators, * categories, * listBox;
    Engine * engine ;
    std::vector <GtkWidget *> * boxes;
    void * rack ;
} Sorter;

typedef void (*HashCommand)(void *);

class Rack {
public:
    json config ;
    std::string theme ;
    Engine * engine ;
    void * presets ;
    std::map <std::string, HashCommand> hashCommands ;
    std::map <int, GtkWidget*> pMap ;
    GtkBox * master, * mixer;
    GtkHeaderBar * button_box ;
    GtkBox * list_box ;
    GtkOverlay * overlay ;
    GtkButton * add_effect ;
    GtkScrolledWindow * sw ;
    std::vector <GtkWidget *> plugs;
    std::vector <PluginUI *> uiv ;
    
    void move_up (PluginUI *);
    void move_down (PluginUI *);
    void build ();

    GtkButton * logo, * menu_button, * patch_up, * patch_down ;
    GtkLabel * current_patch ;
    GtkToggleButton * mixer_toggle, * record ;
    GtkWidget * listBox ;
    GtkSwitch * onoff ;
    GtkToggleButton * toggle_presets ;
    
    bool bnobs = true ;
    int patch = 0;
    
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
    
    void next_preset ();
    void prev_preset ();
};

void rack_clear (GtkWidget * button, Rack * rack) ;

#endif // RACK_H
