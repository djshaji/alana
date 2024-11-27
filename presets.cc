#include "presets.h"

std::string * Presets::presets_dir ;

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
    IN
    Presets * presets = (Presets *) d ;
    gtk_spinner_start (presets->library_spinner);
    std::string lJson = std::string (presets->dir).append ("/").append ("library.json") ;
    download_file ((char *)"https://amprack.in/presets.json", lJson.c_str ());
    gtk_spinner_stop (presets->library_spinner);
    //~ presets->add_preset_multi (lJson, 2);
    presets->page = 0 ;
    presets->library_json = filename_to_json (std::string (presets->dir).append("/library.json"));
    gtk_adjustment_set_upper (presets->adj, presets->library_json.size() / presets->page_size);
    presets->library_load ();
    OUT
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
    for (int i = 0 ; i < 4 ; i ++)
        list_of_presets [i] = new std::vector <json>();
    
    presets = (GtkNotebook *) gtk_notebook_new ();
    gtk_notebook_append_page (notebook, (GtkWidget *)presets, gtk_label_new ("Presets"));
    
    my_presets = (GtkBox *) gtk_box_new (GTK_ORIENTATION_VERTICAL, 10) ;
    quick = (GtkBox *) gtk_box_new (GTK_ORIENTATION_VERTICAL, 10) ;
    library = (GtkBox *) gtk_box_new (GTK_ORIENTATION_VERTICAL, 10) ;
    favorites = (GtkBox *) gtk_box_new (GTK_ORIENTATION_VERTICAL, 10) ;

    gtk_widget_set_name ((GtkWidget *)my_presets, "rack");
    gtk_widget_set_name ((GtkWidget *)quick, "rack");
    gtk_widget_set_name ((GtkWidget *)library, "rack");
    gtk_widget_set_name ((GtkWidget *)favorites, "rack");
    
    GtkLabel * l1 = (GtkLabel * ) gtk_label_new ("User");
    GtkLabel * l2 = (GtkLabel * ) gtk_label_new ("Factory");
    GtkLabel * l3 = (GtkLabel * ) gtk_label_new ("Library");
    GtkLabel * lf = (GtkLabel * ) gtk_label_new ("♥");
    
    GtkScrolledWindow * sw_q = (GtkScrolledWindow *) gtk_scrolled_window_new (), 
        * sw_l = (GtkScrolledWindow *) gtk_scrolled_window_new (),
        * sw_f = (GtkScrolledWindow *) gtk_scrolled_window_new ();
    
    gtk_scrolled_window_set_child (sw_q, (GtkWidget *) quick);
    gtk_scrolled_window_set_child (sw_l, (GtkWidget *) library);
    gtk_scrolled_window_set_child (sw_f, (GtkWidget *) favorites);
    
    GtkBox * lbox = (GtkBox * ) gtk_box_new (GTK_ORIENTATION_VERTICAL, 10);
    
    gtk_notebook_append_page (presets, (GtkWidget *)sw_q, gtk_label_new ("Factory"));
    gtk_notebook_append_page (presets, (GtkWidget *)my_presets, gtk_label_new ("User"));
    gtk_notebook_append_page (presets, (GtkWidget *)lbox, gtk_label_new ("Library"));
    gtk_notebook_append_page (presets, (GtkWidget *)sw_f, gtk_label_new ("♥"));
    
    //~ Gtk::Box bbox = Gtk::Box (Gtk::Orientation::HORIZONTAL, 10);
    add = (GtkButton *)gtk_button_new_with_label ("Save");
    
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

    GtkMenuButton * menu_button = (GtkMenuButton * )gtk_menu_button_new ();
    
    //~ bbox.set_valign (Gtk::Align::END);
    //~ bbox.set_hexpand (true);
    gtk_widget_set_halign ((GtkWidget *) add, GTK_ALIGN_CENTER);
    gtk_widget_set_vexpand ((GtkWidget *) add, false);
    
    GtkBox * hbox = (GtkBox * ) gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 10);
    
    gtk_widget_set_halign ((GtkWidget *) hbox, GTK_ALIGN_CENTER);
    gtk_widget_set_vexpand ((GtkWidget *) hbox, false);

    gtk_widget_set_halign ((GtkWidget *) menu_button, GTK_ALIGN_CENTER);
    gtk_widget_set_vexpand ((GtkWidget *) menu_button, false);
    //~ bbox.append (add);
    
    gtk_menu_button_set_menu_model (menu_button, (GMenuModel *) menu);
    
    my_presets_rack = (GtkBox *) gtk_box_new (GTK_ORIENTATION_VERTICAL, 10);
    GtkScrolledWindow * sw = (GtkScrolledWindow * )gtk_scrolled_window_new ();
    
    //~ sw.append (my_presets_rack);
    gtk_scrolled_window_set_child (sw, (GtkWidget *)my_presets_rack);
    gtk_box_append (my_presets, (GtkWidget *)sw);

    GtkButton * load_f = (GtkButton * )gtk_button_new_with_label ("Import") ;
    g_signal_connect (load_f, "clicked",(GCallback) load_from_file, this);
    
    GtkButton * save_f = (GtkButton *) gtk_button_new_with_label ("Export") ;
    g_signal_connect (save_f, "clicked",(GCallback) save_to_file, this);
    
    GtkButton * refresh = (GtkButton *) gtk_button_new_with_label ("Refresh 🔃") ;
    g_signal_connect (refresh, "clicked",(GCallback) download_cb, this);
    
    GtkButton * next = (GtkButton *) gtk_button_new_with_label (">") ;
    g_signal_connect (next, "clicked",(GCallback) presets_next, this);
    
    //~ pno.set_visible (false);

    library_json = filename_to_json (std::string (dir).append("/library.json"));

    adj = gtk_adjustment_new (1,1,library_json.size() / page_size,1,1,1);
    GtkSpinButton * pno = (GtkSpinButton * )gtk_spin_button_new (adj, 0.1, 2);
    //~ gtk_spin_button_set_adjustment (pno.gobj (), adj);

    g_signal_connect ((GtkWidget *)pno, "value-changed", (GCallback) change_value, this);
    
    page_no = (GtkWidget *)pno ;
    
    GtkButton * prev = (GtkButton * )gtk_button_new_with_label ("<") ;
    g_signal_connect (prev, "clicked",(GCallback) presets_prev, this);

    gtk_widget_set_halign ((GtkWidget *) refresh, GTK_ALIGN_CENTER);
    gtk_widget_set_vexpand ((GtkWidget *) refresh, false);
    gtk_widget_set_vexpand ((GtkWidget *) sw_l, true);

    gtk_box_append (my_presets, (GtkWidget *)hbox);
    //~ lbox.append (refresh);
    
    GtkBox * lvBox =  (GtkBox * )gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 10);
    //~ lvBox.set_margin (10);
    
    gtk_widget_set_halign ((GtkWidget *) lvBox, GTK_ALIGN_CENTER);
    gtk_widget_set_vexpand ((GtkWidget *) lvBox, false);

    library_spinner = (GtkSpinner *)gtk_spinner_new ();
    gtk_widget_show ((GtkWidget *)library_spinner);
    //~ gtk_spinner_set_spinning ((GtkSpinner *) library_spinner, true);
    //~ gtk_spinner_start ((GtkSpinner *) library_spinner);
    
    gtk_box_append (lvBox, (GtkWidget *) prev);
    gtk_box_append (lvBox, (GtkWidget *) refresh);
    gtk_box_append (lvBox, (GtkWidget *) next);
    gtk_box_append (lvBox, (GtkWidget *)library_spinner);
    gtk_box_append (lvBox, (GtkWidget *) pno);
    
    gtk_box_append (lbox, (GtkWidget *) sw_l);
    gtk_box_append (lbox, (GtkWidget *) lvBox);
    
    gtk_box_append (hbox, (GtkWidget *) add);
    gtk_box_append (hbox, (GtkWidget *) load_f);
    gtk_box_append (hbox, (GtkWidget *) save_f);
    //~ hbox.append (menu_button);
    //~ hbox.set_margin (10);
    gtk_widget_set_name ((GtkWidget *) hbox, "header");
    gtk_widget_set_name ((GtkWidget *) lbox, "header");
    //~ gtk_widget_set_name ((GtkWidget *) lbox, "header");
    //~ lbox.set_name ("header");
    //~ add.set_margin (10);
    //~ menu_button.set_margin (10);
    
    gtk_widget_set_vexpand ((GtkWidget *)my_presets_rack, true);
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
    GtkWidget *iter = gtk_widget_get_first_child ((GtkWidget *)library);
    while (iter != NULL) {
      GtkWidget *next = gtk_widget_get_next_sibling ((GtkWidget *)iter);
      gtk_box_remove ((GtkBox *)library, iter);
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
    #ifdef __linux__
    std::string filename = std::string (presets->presets_dir->c_str());
    #else
    std::string filename = std::string ("");
    if (name.find (":\\") == std::string::npos)
        filename.append (presets->presets_dir->c_str ());
    #endif
    
    filename.append (name);
    LOGV (filename.c_str ());
    
    if (! std::filesystem::remove (filename)) {
        LOGD ("[delete preset] failed: %s\n", filename.c_str ()) ;
        return ;
    }
    
    GtkWidget * box = gtk_widget_get_parent (button) ;
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
    list_of_presets [which]->push_back (j);
    //~ IN
    //~ std::cout << j << std::endl;
    GtkBox * h = (GtkBox * )gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 10) ;
    GtkBox * h2 = (GtkBox * )gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 10) ;
    GtkBox * v = (GtkBox * )gtk_box_new (GTK_ORIENTATION_VERTICAL, 10) ;
    GtkBox * v2 = (GtkBox * )gtk_box_new (GTK_ORIENTATION_VERTICAL, 10) ;
    
    //~ v.set_margin (10);
    gtk_box_append (h, (GtkWidget *) v2);
    
    //~ GtkSeparator * sep = Gtk::Separator ();
    //~ sep.set_hexpand (true);
    gtk_widget_set_hexpand ((GtkWidget *)h, true);
    gtk_widget_set_hexpand ((GtkWidget *)v2, true);
    //~ h.append (sep);
    gtk_box_append (v, (GtkWidget *) h);
    gtk_box_append (v, (GtkWidget *) h2);

    gtk_widget_set_name ((GtkWidget *) v, "plugin");
       
    switch (which) {
        case 0:
            gtk_box_append (quick, (GtkWidget *) v);
            break ;
        case 2:
            gtk_box_append (library, (GtkWidget *) v);            
            library_boxes.push_back ((GtkWidget *)v);
            break ;
        case 1:
            gtk_box_append (my_presets_rack, (GtkWidget *) v);            
            break;
        case 3:
            gtk_box_append (favorites, (GtkWidget *) v);            
            break;
    }
    
    GtkToggleButton * fav = (GtkToggleButton *)gtk_toggle_button_new_with_label ("♥");
        
    GtkLabel * title = (GtkLabel *  )gtk_label_new ("");
    gtk_label_set_wrap (title, true);
    gtk_label_set_max_width_chars (title, 20);
    gtk_label_set_markup (title, std::string ("<big><b>").append (j ["name"]).append ( "</b></big>").c_str ());
    GtkLabel * desc = (GtkLabel *)gtk_label_new (j ["desc"].dump ().c_str ());
    gtk_label_set_wrap (desc, true);
    gtk_label_set_max_width_chars (desc, 20);
    
    //~ title.set_hexpand (true);
    gtk_widget_set_hexpand ((GtkWidget *) h, true);
    gtk_label_set_justify (title, GTK_JUSTIFY_LEFT);
    
    GtkBox * tb = (GtkBox *) gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_box_append (v2, (GtkWidget *)tb);
    GtkButton * bt = (GtkButton * )gtk_button_new ();
    gtk_widget_set_name ((GtkWidget *) bt, "effect-button");
    gtk_button_set_child (bt, (GtkWidget *) title);
    gtk_box_append (tb, GTK_WIDGET (bt));
    gtk_box_append (tb, GTK_WIDGET (fav));
    gtk_widget_set_hexpand ((GtkWidget *)title, true);
    gtk_widget_set_halign ((GtkWidget *)title, GTK_ALIGN_START);
    // todo: description
    if (j ["desc"].dump () != std::string ("\"\"")) {
        GtkBox * tb = (GtkBox *) gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 10);
        gtk_box_append (v2, (GtkWidget *) tb);
        gtk_box_append (tb, (GtkWidget *) desc);
        gtk_label_set_natural_wrap_mode (desc, GTK_NATURAL_WRAP_WORD);
    }
   
    GtkButton * load = (GtkButton *) gtk_button_new_with_label ("Load");
    GtkButton * del = (GtkButton *)gtk_button_new_with_label ("Delete");
    std::string _name = j ["name"].dump().c_str () ;
    _name = _name.substr (1, _name.size () - 2);
    gtk_widget_set_name ((GtkWidget *) del, _name.c_str ());
    
    g_signal_connect (del, "clicked", (GCallback)delete_callback, this);
    
    //~ h2.append (load);

    CB_Preset * cb = new CB_Preset ();
    cb -> engine = engine ;
    cb->filename = std::string (dir).append ("/favs/").append (_name);
    cb -> rack = rack ;
    cb -> j = j ;
    cb -> fav = (GtkWidget *)fav ;
    cb -> fav_rack = (GtkWidget *) favorites;
    cb -> p = (void *) this ;
    
    if (g_file_test (cb->filename.c_str (), G_FILE_TEST_EXISTS))
        gtk_toggle_button_set_active ((GtkToggleButton *) fav, true);
    
    g_signal_connect (load, "clicked", (GCallback) load_preset_cb, cb);
    g_signal_connect (bt, "clicked", (GCallback) load_preset_cb, cb);
    g_signal_connect (fav, "toggled", (GCallback) preset_fav_cb, cb);

    gtk_widget_set_halign ((GtkWidget *) load, GTK_ALIGN_END);
    
    //~ v.append (h2);
    if (which == 1)
        gtk_box_append (v, (GtkWidget *) del);
    //~ h2.append (fav);
    gtk_widget_set_halign ((GtkWidget *) del, GTK_ALIGN_CENTER);
    //~ OUT
}

void Presets::load_user (bool isFav) {
    IN
    std::string where = std::string (presets_dir->c_str ()) ;
    if (isFav)
        where = favs_dir ;
        
    for (const auto & entry : std::filesystem::directory_iterator(where)) {
        //~ std::cout << entry.path() << std::endl;
        LOGD ("[preset] %s\n", entry.path ().string ().c_str ());
    
        json j = filename_to_json (entry.path ().string ());
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

int Presets::import_presets_from_json (json j) {
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
        LOGD ("[presets] warning: not adding preset to UI\n");
        # ifdef __linux__
            add_preset (preset, 1);
        # endif
    }
    
    msg (std::string ("Imported ").append (std::to_string (how_many)).append (" presets successfully."));
    return how_many ;
}

void Presets::save_presets_to_json (std::string filename) {
    json ex = json {};
    int i = 0 ;
    for (const auto & entry : std::filesystem::directory_iterator(*presets_dir)) {
        //~ std::cout << entry.path() << std::endl;
        json j = filename_to_json (entry.path ().string ());
        ex [std::to_string (i++)] = j ;
    } 
    
    if (json_to_filename (ex, filename))
        msg ("Exported presets successfully");
    else 
        msg ("Error exporting presets");
}

json Presets::get_all_user_presets () {
    IN
    json ex = json {};
    int i = 0 ;
    for (const auto & entry : std::filesystem::directory_iterator(*presets_dir)) {
        //~ std::cout << entry.path() << std::endl;
        json j = filename_to_json (entry.path ().string ());
        LOGD ("[preset] %s: %s\n", entry.path ().string ().c_str (), j.dump().c_str ());
        ex [std::to_string (i++)] = j ;
    } 

    OUT
    return ex ;
}
