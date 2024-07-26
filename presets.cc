#include "presets.h"
void load_preset_cb (void * c, void * d) {
    CB_Preset * cb = (CB_Preset *) d ;
    //~ cb -> engine -> load_preset (cb -> j) ;    
    cb -> rack -> load_preset (cb -> j);
    
}

void Presets::my () {
    presets = Gtk::Notebook () ;
    notebook.append_page (presets, "Presets");
    
    my_presets = Gtk::Box (Gtk::Orientation::VERTICAL, 10) ;
    quick = Gtk::Box (Gtk::Orientation::VERTICAL, 10) ;
    library = Gtk::Box (Gtk::Orientation::VERTICAL, 10) ;
    
    Gtk::Label l1 = Gtk::Label ("My Presets");
    Gtk::Label l2 = Gtk::Label ("Quick Presets");
    Gtk::Label l3 = Gtk::Label ("Library");
    
    Gtk::ScrolledWindow sw_q = Gtk::ScrolledWindow (), 
        sw_l = Gtk::ScrolledWindow () ;
    
    gtk_scrolled_window_set_child (sw_q.gobj (), (GtkWidget *) quick.gobj ());
    gtk_scrolled_window_set_child (sw_l.gobj (), (GtkWidget *) library.gobj ());
    
    presets.append_page (sw_q, l2);
    presets.append_page (my_presets, l1);
    presets.append_page (sw_l, l3);
    
    //~ Gtk::Box bbox = Gtk::Box (Gtk::Orientation::HORIZONTAL, 10);
    add = Gtk::Button ("Save");
    //~ bbox.set_valign (Gtk::Align::END);
    //~ bbox.set_hexpand (true);
    add.set_halign (Gtk::Align::CENTER);
    add.set_vexpand (false);
    //~ bbox.append (add);
    
    my_presets_rack = Gtk::Box (Gtk::Orientation::VERTICAL, 10);
    Gtk::ScrolledWindow sw = Gtk::ScrolledWindow () ;
    
    //~ sw.append (my_presets_rack);
    gtk_scrolled_window_set_child (sw.gobj (), (GtkWidget *)my_presets_rack.gobj ());
    my_presets.append (sw);
    
    my_presets.append (add);
    add.set_margin (10);
    
    my_presets_rack.set_vexpand (true);
    //~ add_preset (1) ;
    load_user ();
    add_preset_multi (std::string ("quick.json"), 0);
}


void delete_callback (void * b, void * d) {
    IN
    GtkWidget * button = (GtkWidget *) b ;
    Presets * presets = (Presets *) d ;
    std::string name = std::string (gtk_widget_get_name (button));
    LOGV (name.c_str ());
    std::string filename = std::string (presets->presets_dir);
    filename.append (name);
    LOGV (filename.c_str ());
    
    if (! std::filesystem::remove (filename)) {
        LOGD ("[delete preset] failed: %s\n", filename.c_str ()) ;
        return ;
    }
    
    GtkWidget * box = gtk_widget_get_parent (gtk_widget_get_parent (button)) ;
    gtk_box_remove ((GtkBox *)gtk_widget_get_parent (box), box);
    
    //~ alert_yesno (name, "Do you want to delete this preset?", null, null);
    OUT
}

void Presets::add_preset (json j, int which) {
    IN
    std::cout << j << std::endl;
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
        case 0:
            quick.append (v);
            break ;
        case 2:
            library.append (v);
            break ;
        case 1:
            my_presets_rack.append (v);
            break;
    }
    
    Gtk::Label title = Gtk::Label () ;
    title.set_markup (std::string ("<big><b>").append (j ["name"]).append ( "</b></big>").c_str ());
    Gtk::Label desc = Gtk::Label (j ["desc"].dump ().c_str ());
    
    //~ title.set_hexpand (true);
    h.set_hexpand (true);
    title.set_justify (Gtk::Justification::LEFT);
    
    Gtk::Box tb = Gtk::Box (Gtk::Orientation::HORIZONTAL, 10);
    v2.append (tb);
    tb.append (title);
    // todo: description
    if (j ["desc"].dump () != std::string ("\"\"")) {
        Gtk::Box tb = Gtk::Box (Gtk::Orientation::HORIZONTAL, 10);
        v2.append (tb);
        tb.append (desc);
    }
   
    Gtk::Button load = Gtk::Button ("Load");
    Gtk::Button del = Gtk::Button ("Delete");
    std::string _name = j ["name"].dump().c_str () ;
    _name = _name.substr (1, _name.size () - 2);
    del.set_name (_name);
    
    g_signal_connect (del.gobj (), "clicked", (GCallback)delete_callback, this);
    
    h.append (load);
    
    CB_Preset * cb = new CB_Preset ();
    cb -> engine = engine ;
    cb -> rack = rack ;
    cb -> j = j ;
    
    g_signal_connect (load.gobj (), "clicked", (GCallback) load_preset_cb, cb);
    
    load.set_halign (Gtk::Align::END);
    
    //~ v.append (h2);
    h.append (del);
    del.set_halign (Gtk::Align::CENTER);
    OUT
}

void Presets::load_user () {
    IN
    for (const auto & entry : std::filesystem::directory_iterator(presets_dir)) {
        std::cout << entry.path() << std::endl;
    
        json j = filename_to_json (entry.path ());
        add_preset (j, 1);
    } 
    OUT   
}

void Presets::add_preset_multi (std::string s, int which) {
    IN
    add_preset_multi (filename_to_json (s), which);
    OUT
}

void Presets::add_preset_multi (json j, int which) {
    IN
    for (auto n: j) {
        add_preset (n, which);
    }
    
    OUT
}
