#include "presets.h"

void
change_value (
  GtkSpinButton* self,
  gpointer user_data
) {
    Presets * p = (Presets *) user_data ;
    p->page = gtk_adjustment_get_value (p -> adj);
    p->library_load () ;
}

void load_preset_cb (void * c, void * d) {
    CB_Preset * cb = (CB_Preset *) d ;
    //~ cb -> engine -> load_preset (cb -> j) ;    
    cb -> rack -> load_preset (cb -> j);
    
}

void download_cb (void * w, void * d) {
    ///>    TODO: 
    //      clear box before refresh
    Presets * presets = (Presets *) d ;
    gtk_spinner_start (presets->library_spinner);
    std::string lJson = std::string (presets->dir).append ("/").append ("library.json") ;
    download_file ("https://amprack.in/presets.json", lJson.c_str ());
    gtk_spinner_stop (presets->library_spinner);
    //~ presets->add_preset_multi (lJson, 2);
    presets->page = 0 ;
    presets->library_json = filename_to_json (std::string (presets->dir).append("/library.json"));
    gtk_adjustment_set_upper (presets->adj, presets->library_json.size() / presets->page_size);
    presets->library_load ();
}

void presets_on_response (GtkNativeDialog *native,
             int              response, gpointer data)
{
    if (response == GTK_RESPONSE_ACCEPT)
    {
        GtkFileChooser *chooser = GTK_FILE_CHOOSER (native);
        GFile *file = gtk_file_chooser_get_file (chooser);
        Presets * presets = (Presets *) data;
        char * filename = g_file_get_path (file);
        
        LOGV (filename);
        //~ LOGV (presets->_pdir);
        presets-> import_presets_from_json (filename_to_json (std::string (filename))) ;

        free (filename);
        g_object_unref (file);
    }

  g_object_unref (native);
}

void load_from_file (void * b, void * d) {
  GtkFileChooserNative *native;
  GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_OPEN;

  native = gtk_file_chooser_native_new ("Open File",
                                        NULL,
                                        action,
                                        "_Open",
                                        "_Cancel");

  g_signal_connect (native, "response", G_CALLBACK (presets_on_response), d);
  gtk_native_dialog_show (GTK_NATIVE_DIALOG (native));


}


void presets_off_response (GtkNativeDialog *native,
             int              response, gpointer data)
{
    if (response == GTK_RESPONSE_ACCEPT)
    {
        GtkFileChooser *chooser = GTK_FILE_CHOOSER (native);
        GFile *file = gtk_file_chooser_get_file (chooser);
        Presets * presets = (Presets *) data;
        char * filename = g_file_get_path (file);
        
        LOGV (filename);
        //~ LOGV (presets->_pdir);
        presets-> save_presets_to_json (std::string (filename)) ;

        free (filename);
        g_object_unref (file);
    }

  g_object_unref (native);
}

void save_to_file (void * b, void * d) {
  GtkFileChooserNative *native;
  GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_SAVE;

  native = gtk_file_chooser_native_new ("Save File",
                                        NULL,
                                        action,
                                        "_Open",
                                        "_Cancel");

  g_signal_connect (native, "response", G_CALLBACK (presets_off_response), d);
  gtk_native_dialog_show (GTK_NATIVE_DIALOG (native));


}

void menu_clicked (void * action, void * data) {
    
}

void presets_next (void * a, void * d) {
    Presets * p = (Presets *) d ;
    p -> page ++ ;
    gtk_adjustment_set_value (p -> adj, p -> page);
    //~ p -> library_load ();
}

void presets_prev (void * a, void * d) {
    Presets * p = (Presets *) d ;
    p -> page -- ;
    
    if (p -> page < 0)
        p -> page = 0 ;
    
    gtk_adjustment_set_value (p -> adj, p -> page);
}

void Presets::my () {
    IN
    presets = Gtk::Notebook () ;
    notebook.append_page (presets, "Presets");
    
    my_presets = Gtk::Box (Gtk::Orientation::VERTICAL, 10) ;
    quick = Gtk::Box (Gtk::Orientation::VERTICAL, 10) ;
    library = Gtk::Box (Gtk::Orientation::VERTICAL, 10) ;
    favorites = Gtk::Box (Gtk::Orientation::VERTICAL, 10) ;
    
    Gtk::Label l1 = Gtk::Label ("My Presets");
    Gtk::Label l2 = Gtk::Label ("Quick Presets");
    Gtk::Label l3 = Gtk::Label ("Library");
    Gtk::Label lf = Gtk::Label ("Favorites");
    
    Gtk::ScrolledWindow sw_q = Gtk::ScrolledWindow (), 
        sw_l = Gtk::ScrolledWindow (),
        sw_f = Gtk::ScrolledWindow () ;
    
    gtk_scrolled_window_set_child (sw_q.gobj (), (GtkWidget *) quick.gobj ());
    gtk_scrolled_window_set_child (sw_l.gobj (), (GtkWidget *) library.gobj ());
    gtk_scrolled_window_set_child (sw_f.gobj (), (GtkWidget *) favorites.gobj ());
    
    Gtk::Box lbox = Gtk::Box (Gtk::Orientation::VERTICAL, 10);
    
    presets.append_page (sw_q, l2);
    presets.append_page (my_presets, l1);
    presets.append_page (lbox, l3);
    presets.append_page (sw_f, lf);
    
    //~ Gtk::Box bbox = Gtk::Box (Gtk::Orientation::HORIZONTAL, 10);
    add = Gtk::Button ("Save");
    
    GMenu * menu = g_menu_new ();

    GSimpleAction * import_action = g_simple_action_new ("import", null);
    GSimpleAction * export_action = g_simple_action_new ("export", null);

    g_action_map_add_action ( G_ACTION_MAP ( app ), G_ACTION ( import_action ) );
    g_action_map_add_action ( G_ACTION_MAP ( app ), G_ACTION ( export_action ) );

    g_signal_connect (import_action,    "activate", G_CALLBACK (load_from_file), this);
    g_signal_connect (export_action, "activate", G_CALLBACK (menu_clicked), NULL);

    GMenuItem * import_from_file = g_menu_item_new ("Import from file", "app.import");
    GMenuItem * export_to_file = g_menu_item_new ("Export to file", "app.export");
    
    g_menu_append_item (menu, import_from_file);
    g_menu_append_item (menu, export_to_file);

    Gtk::MenuButton menu_button = Gtk::MenuButton () ;
    
    //~ bbox.set_valign (Gtk::Align::END);
    //~ bbox.set_hexpand (true);
    add.set_halign (Gtk::Align::CENTER);
    add.set_vexpand (false);
    
    Gtk::Box hbox = Gtk::Box (Gtk::Orientation::HORIZONTAL, 10);
    
    hbox.set_halign (Gtk::Align::CENTER);
    hbox.set_vexpand (false);

    menu_button.set_halign (Gtk::Align::CENTER);
    menu_button.set_vexpand (false);
    //~ bbox.append (add);
    
    gtk_menu_button_set_menu_model (menu_button.gobj (), (GMenuModel *) menu);
    
    my_presets_rack = Gtk::Box (Gtk::Orientation::VERTICAL, 10);
    Gtk::ScrolledWindow sw = Gtk::ScrolledWindow () ;
    
    //~ sw.append (my_presets_rack);
    gtk_scrolled_window_set_child (sw.gobj (), (GtkWidget *)my_presets_rack.gobj ());
    my_presets.append (sw);

    Gtk::Button load_f = Gtk::Button ("Import") ;
    g_signal_connect (load_f.gobj (), "clicked",(GCallback) load_from_file, this);
    
    Gtk::Button save_f = Gtk::Button ("Export") ;
    g_signal_connect (save_f.gobj (), "clicked",(GCallback) save_to_file, this);
    
    Gtk::Button refresh = Gtk::Button ("Refresh") ;
    g_signal_connect (refresh.gobj (), "clicked",(GCallback) download_cb, this);
    
    Gtk::Button next = Gtk::Button ("Next") ;
    g_signal_connect (next.gobj (), "clicked",(GCallback) presets_next, this);
    
    Gtk::SpinButton pno = Gtk::SpinButton () ;
    library_json = filename_to_json (std::string (dir).append("/library.json"));

    adj = gtk_adjustment_new (1,1,library_json.size() / page_size,1,1,1);
    gtk_spin_button_set_adjustment (pno.gobj (), adj);

    g_signal_connect ((GtkWidget *)pno.gobj (), "value-changed", (GCallback) change_value, this);
    
    page_no = (GtkWidget *)pno.gobj () ;
    
    Gtk::Button prev = Gtk::Button ("Back") ;
    g_signal_connect (prev.gobj (), "clicked",(GCallback) presets_prev, this);
    
    refresh.set_halign (Gtk::Align::CENTER);
    refresh.set_vexpand (false);
    sw_l.set_vexpand (true);

    my_presets.append (hbox);
    //~ lbox.append (refresh);
    
    Gtk::Box lvBox = Gtk::Box (Gtk::Orientation::HORIZONTAL, 10);
    lvBox.set_margin (10);
    
    lvBox.set_halign (Gtk::Align::CENTER);
    lvBox.set_vexpand (false);

    library_spinner = (GtkSpinner *)gtk_spinner_new ();
    gtk_widget_show ((GtkWidget *)library_spinner);
    //~ gtk_spinner_set_spinning ((GtkSpinner *) library_spinner, true);
    //~ gtk_spinner_start ((GtkSpinner *) library_spinner);
    
    lvBox.append (prev);
    lvBox.append (refresh);
    lvBox.append (next);
    gtk_box_append (lvBox.gobj (), (GtkWidget *)library_spinner);
    lvBox.append (pno);
    
    lbox.append (sw_l);
    lbox.append (lvBox);  
    
    hbox.append (add);
    hbox.append (load_f);
    hbox.append (save_f);
    //~ hbox.append (menu_button);
    hbox.set_margin (10);
    //~ add.set_margin (10);
    //~ menu_button.set_margin (10);
    
    my_presets_rack.set_vexpand (true);
    //~ add_preset (1) ;
    load_user (false);
    load_user (true);
    add_preset_multi (std::string ("assets/quick.json"), 0);
    //~ add_preset_multi (std::string (dir).append("/library.json"), 2);
    library_load ();
    OUT
}


void Presets::library_load () {
    IN
    int x = 0 ;
    GtkWidget *iter = gtk_widget_get_first_child ((GtkWidget *)library.gobj ());
    while (iter != NULL) {
      GtkWidget *next = gtk_widget_get_next_sibling ((GtkWidget *)iter);
      gtk_box_remove ((GtkBox *)library.gobj (), iter);
      iter = next;
      x ++ ;
    }
    
    //~ LOGD ("removed %d boxes\n", x);
    
    library_boxes.clear () ;
    
    int i = 0, e = 0 ; 
    int skip = page * page_size ;
    //~ LOGD ("library: %s\n", library_json.dump ().c_str ());
    for (json p: library_json) {
        if (i < skip) {
            i++ ;
            continue ;
        }
        
        //~ LOGD ("[%d: %d] %d: %s\n", page, skip, e, p ["name"].dump ().c_str ());
        e ++ ;
        if (e > page_size)
            break ;
        
        add_preset (p, 2);
    }
    
    //~ gtk_adjustment_set_value ((GtkAdjustment *)adj, page + 1);
    OUT
}

void delete_callback (void * b, void * d) {
    IN
    GtkWidget * button = (GtkWidget *) b ;
    Presets * presets = (Presets *) d ;
    std::string name = std::string (gtk_widget_get_name (button));
    LOGV (name.c_str ());
    std::string filename = std::string (presets->presets_dir->c_str());
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

void preset_fav_cb (void * b, void * c) {
    CB_Preset * cb = (CB_Preset *) c ;
    GtkToggleButton * t = (GtkToggleButton *) b ;
    Presets * p = (Presets *) cb -> p ;
    
    if (gtk_toggle_button_get_active (t)) {
        json_to_filename (cb -> j, cb->filename.c_str ());
        p -> add_preset (cb ->j, 3);
    } else {
        GtkWidget * box = gtk_widget_get_parent (gtk_widget_get_parent ((GtkWidget *) t)) ;
        gtk_box_remove ((GtkBox *)cb -> fav_rack, box);
        remove (cb->filename.c_str ());
    }
}

void Presets::add_preset (json j, int which) {
    //~ IN
    //~ std::cout << j << std::endl;
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
            library_boxes.push_back ((GtkWidget *)v.gobj ());
            break ;
        case 1:
            my_presets_rack.append (v);
            break;
        case 3:
            favorites.append (v);
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

    Gtk::ToggleButton fav = Gtk::ToggleButton () ;
    fav.set_label ("♥");
    
    CB_Preset * cb = new CB_Preset ();
    cb -> engine = engine ;
    cb->filename = std::string (dir).append ("/favs/").append (_name);
    cb -> rack = rack ;
    cb -> j = j ;
    cb -> fav = (GtkWidget *)fav.gobj () ;
    cb -> fav_rack = (GtkWidget *) favorites.gobj ();
    cb -> p = (void *) this ;
    
    if (g_file_test (cb->filename.c_str (), G_FILE_TEST_EXISTS))
        fav.set_active (true);
    
    g_signal_connect (load.gobj (), "clicked", (GCallback) load_preset_cb, cb);
    g_signal_connect (fav.gobj (), "toggled", (GCallback) preset_fav_cb, cb);
    
    load.set_halign (Gtk::Align::END);
    
    //~ v.append (h2);
    if (which == 1)
        v.append (del);
    h.append (fav);
    del.set_halign (Gtk::Align::CENTER);
    //~ OUT
}

void Presets::load_user (bool isFav) {
    IN
    std::string where = std::string (presets_dir->c_str ()) ;
    if (isFav)
        where = favs_dir ;
        
    for (const auto & entry : std::filesystem::directory_iterator(where)) {
        //~ std::cout << entry.path() << std::endl;
    
        json j = filename_to_json (entry.path ());
        if (! isFav)
            add_preset (j, 1);
        else
            add_preset (j, 3);
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

void Presets::import_presets_from_json (json j) {
    IN
    int how_many = 0 ;
    LOGV (presets_dir->c_str());
    for (auto preset: j) {
        how_many ++ ;
        std::string basename = preset ["name"].dump () ;
        basename = basename.substr (1, basename.size () - 2);
        std::string filename = std::string (presets_dir->c_str ()) .append (basename) ;
        wtf ("writing file: %s\n", filename.c_str ());
        json_to_filename (preset, filename) ;
        add_preset (preset, 1);
    }
    
    msg (std::string ("Imported ").append (std::to_string (how_many)).append (" presets successfully."));
}

void Presets::save_presets_to_json (std::string filename) {
    json ex = json {};
    int i = 0 ;
    for (const auto & entry : std::filesystem::directory_iterator(*presets_dir)) {
        //~ std::cout << entry.path() << std::endl;
        json j = filename_to_json (entry.path ());
        ex [std::to_string (i++)] = j ;
    } 
    
    if (json_to_filename (ex, filename))
        msg ("Exported presets successfully");
    else 
        msg ("Error exporting presets");
}
