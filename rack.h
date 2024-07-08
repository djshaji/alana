#ifndef RACK_H
#define RACK_H

#include <gtkmm.h>
#include <vector>
#include <iostream>

#include "json.hpp"
#include "log.h"
#include "defs.h"
#include "engine.h"

class PluginUI {
    std::vector <Gtk::Scale> sliders ;
    Gtk::Label name ;
    Gtk::ToggleButton onoff ;
    Gtk::Button del ;
    
} ;

class Rack {
public:
    Engine * engine ;
    Gtk::Box master, button_box, mixer;
    Gtk::ListBox list_box ;
    Gtk::Overlay overlay ;
    Gtk::Button add_effect ;
    
    Gtk::Button logo, menu_button ;
    Gtk::ToggleButton mixer_toggle, onoff, record ;
        
    void add ();

    Rack () {
        engine = new Engine () ;
        
        master = Gtk::Box ();
        button_box = Gtk::Box ();
        mixer = Gtk::Box ();
        
        list_box = Gtk::ListBox () ;
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
        button_box.set_orientation (Gtk::Orientation::HORIZONTAL);

        master.append (button_box);
        master.append (mixer);
        master.append (list_box);
        master.append (add_effect);
        add_effect.set_label ("+ Effect");
        
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
             
    }
};

#endif // RACK_H
