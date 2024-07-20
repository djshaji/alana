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
    std::vector <GtkWidget *> boxes;
} Sorter;

class Rack {
public:
    Engine * engine ;
    std::map <int, GtkWidget*> pMap ;
    Gtk::Box master, button_box, mixer;
    Gtk::Box list_box ;
    Gtk::Overlay overlay ;
    Gtk::Button add_effect ;
    Gtk::ScrolledWindow sw ;
    
    Gtk::Button logo, menu_button ;
    Gtk::ToggleButton mixer_toggle, onoff, record ;
    GtkWidget * listBox ;
    
    std::vector <PluginUI> plugins ;        
    void add ();
    
    GtkWidget * pluginDialog ;
    void createPluginDialog () ;
    GtkWidget * addPluginEntry (std::string plug) ;
    
    json blacklist ;
    
    Rack () {
        engine = new Engine () ;
        blacklist = filename_to_json ("assets/blacklist.json");
        
        //~ for (auto b = blacklist.begin () ; b != blacklist.end () ; b ++) {
            //~ LOGD ("[blacklist] %s\n", b.key ()) ;
        //~ }
        master = Gtk::Box ();
        button_box = Gtk::Box ();
        mixer = Gtk::Box ();
        
        list_box = Gtk::Box (Gtk::Orientation::VERTICAL, 10) ;
        sw = Gtk::ScrolledWindow () ;
        overlay = Gtk::Overlay ();
        
        add_effect = Gtk::Button () ;
        logo = Gtk::Button () ;
        logo.set_label ("Amp Rack");
        menu_button = Gtk::Button () ;
        menu_button.set_label (":");
        
        mixer_toggle = Gtk::ToggleButton ();
        mixer_toggle.set_label ("Mixer");
        onoff = Gtk::ToggleButton ();
        onoff.set_label ("On");
        record = Gtk::ToggleButton ();
        record.set_label ("Rec");
        
        master.set_orientation (Gtk::Orientation::VERTICAL);
        master.set_vexpand (true);
        list_box.set_vexpand (true);
        master.set_hexpand (true);
        button_box.set_orientation (Gtk::Orientation::HORIZONTAL);

        list_box.set_orientation (Gtk::Orientation::VERTICAL);
        // list_box.set_visible (false);
        master.append (button_box);
        master.append (mixer);
        master.append (sw);
        sw.set_child (list_box);
        list_box.set_name ("rack");
        Gtk::Separator sep = Gtk::Separator () ;
        // list_box.append (sep);
        // sep.set_vexpand (true);
        master.append (add_effect);
        add_effect.set_label ("+ Effect");
        add_effect.set_valign (Gtk::Align::END);
        // list_box.set_valign (Gtk::Align::END);
        // sw.set_propagate_natural_height (true);
        add_effect.set_halign (Gtk::Align::CENTER);
        
        add_effect.signal_clicked().connect(sigc::mem_fun(*this,
              &Rack::add));

        button_box.set_halign (Gtk::Align::CENTER);
        button_box.set_hexpand (true);
        
        button_box.append (logo);
        button_box.append (mixer_toggle);
        logo.set_halign (Gtk::Align::START);
        mixer_toggle.set_halign (Gtk::Align::END);
        button_box.append (onoff);
        button_box.append (record);
        button_box.append (menu_button);
        
        //~ overlay.set_child (master);
        //~ overlay.add_overlay (add_effect);
        //~ overlay.set_clip_overlay (add_effect, false);
        //~ overlay.set_measure_overlay (add_effect, false);
        
        //~ overlay.set_halign (Gtk::Align::END);
        //~ overlay.set_valign (Gtk::Align::END);
        //~ add_effect.set_label ("+ Effect");

        createPluginDialog () ;
    }
};

#endif // RACK_H
