#include <gtkmm.h>
#include <vector>
#include <iostream>

#include "json.hpp"
#include "log.h"
#include "defs.h"
#include "engine.h"
#include "Plugin.h"
#include "PluginControl.h"

class PluginUI {
public:    
    Gtk::Label name ;
    Gtk::ToggleButton  onoff ;
    Gtk::Button  del,  up,  down ;
    Engine * engine ;
    Plugin * plugin ;
    Gtk::Box  card ;
    
    PluginUI (Engine * _engine, Plugin * _plugin, std::string pluginName) {
        engine = _engine ;
        plugin = _plugin ;

        name =  Gtk::Label (pluginName) ;
        onoff =  Gtk::ToggleButton ();
        card =  Gtk::Box (Gtk::Orientation::VERTICAL, 0);
        card.set_orientation (Gtk::Orientation::VERTICAL);
        card.append (name);
        card.set_margin (10);

        for (int i = 0 ; i < plugin->pluginControls.size () ; i ++) {
            PluginControl * control = plugin->pluginControls.at (i) ;
            Gtk::Scale  scale =  Gtk::Scale () ;
            scale . set_value (control->val);
            scale.set_hexpand (true);

            // Gtk::Adjustment  adj =  Gtk::Adjustment (control->val, control->min, control->max, 1, 1, 1);
            Gtk::SpinButton  spin =  Gtk::SpinButton ();

            Gtk::Label  label =  Gtk::Label (control->name);
            Gtk::Box  box =  Gtk::Box (Gtk::Orientation::HORIZONTAL, 10);
            
            box.set_orientation (Gtk::Orientation::HORIZONTAL);
            box.append (label);
            box.append (scale);
            box.append (spin);
            box.set_spacing (0);

            card.append (box);
        }
    }
} ;
