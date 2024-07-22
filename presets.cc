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
    
    //~ Gtk::Box bbox = Gtk::Box (Gtk::Orientation::HORIZONTAL, 10);
    add = Gtk::Button ("Save");
    //~ bbox.set_valign (Gtk::Align::END);
    //~ bbox.set_hexpand (true);
    add.set_halign (Gtk::Align::CENTER);
    add.set_vexpand (false);
    //~ bbox.append (add);
    
    my_presets_rack = Gtk::Box (Gtk::Orientation::VERTICAL, 10);
    my_presets.append (my_presets_rack);
    my_presets.append (add);
    add.set_margin (10);
    
    my_presets_rack.set_vexpand (true);
    add_preset (1) ;
}

void Presets::add_preset (int which) {
    Gtk::Box h = Gtk::Box (Gtk::Orientation::HORIZONTAL, 10) ;
    Gtk::Box h2 = Gtk::Box (Gtk::Orientation::HORIZONTAL, 10) ;
    Gtk::Box v = Gtk::Box (Gtk::Orientation::VERTICAL, 10) ;
    Gtk::Box v2 = Gtk::Box (Gtk::Orientation::VERTICAL, 10) ;
    
    v.set_margin (10);
    h.append (v2);
    
    Gtk::Separator sep = Gtk::Separator ();
    sep.set_hexpand (true);
    h.set_hexpand (true);
    v2.set_hexpand (true);
    //~ h.append (sep);
    v.append (h);
    v.append (h2);
       
    switch (which) {
        case 1:
            my_presets_rack.append (v);
            break;
    }
    
    Gtk::Label title = Gtk::Label ("<big><b>Rock and roll</b></big>") ;
    title.set_markup ("<big><b>Rock and roll</b></big>");
    Gtk::Label desc = Gtk::Label ("This is an example preset");
    
    //~ title.set_hexpand (true);
    h.set_hexpand (true);
    title.set_justify (Gtk::Justification::LEFT);
    
    v2.append (title);
    v2.append (desc);
   
    Gtk::Button load = Gtk::Button ("Load");
    Gtk::Button del = Gtk::Button ("Delete");
    h.append (load);
    
    load.set_halign (Gtk::Align::END);
    
    v.append (h2);
    h2.append (del);
    del.set_halign (Gtk::Align::CENTER);
}
