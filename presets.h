#ifndef PRESETS_H
#define PRESETS_H
#include <gtkmm.h>

class Presets {
public:
    Gtk::Box master ;
    Gtk::Notebook notebook ;
    Gtk::Button add ;
    
    Presets () {
        master = Gtk::Box () ;
        master.set_orientation (Gtk::Orientation::VERTICAL);
        
        notebook = Gtk::Notebook () ;
        master.append (notebook);
        
        add = Gtk::Button ();
        add.set_label ("+ Preset");
        master.append (add);
    }
};

#endif
