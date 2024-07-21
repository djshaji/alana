#include "presets.h"

void Presets::my () {
    presets = Gtk::Notebook () ;
    notebook.append_page (presets, "Presets");
    
    my_presets = Gtk::Box (Gtk::Orientation::VERTICAL, 10) ;
    quick = Gtk::Box (Gtk::Orientation::VERTICAL, 10) ;
    library = Gtk::Box (Gtk::Orientation::VERTICAL, 10) ;
    
    Gtk::Label l1 = Gtk::Label ("My Presets");
    Gtk::Label l2 = Gtk::Label ("Quick Presets");
    Gtk::Label l3 = Gtk::Label ("Library");
    
    presets.append_page (quick, l2);
    presets.append_page (my_presets, l1);
    presets.append_page (library, l3);
    
    Gtk::Box bbox = Gtk::Box (Gtk::Orientation::HORIZONTAL, 10);
    Gtk::Button add = Gtk::Button ("Save");
    bbox.set_valign (Gtk::Align::END);
    add.set_vexpand (false);
    bbox.append (add);
    my_presets.append (bbox);
    
}
