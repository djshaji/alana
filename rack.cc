#include "rack.h"
#include "presets.h"

void toggle_effects (GtkToggleButton * button, Rack * rack) {
    if (gtk_toggle_button_get_active (button)) {
        gtk_paned_set_position (600) ;
    } else {
        gtk_paned_set_position (100) ;
    }
}

void toggle_record (GtkToggleButton * button, Engine * engine) {
    if (gtk_toggle_button_get_active (button)) {
        engine -> startRecording ();
    } else {
        engine -> stopRecording () ;
    }
}

void preset_next (void * b, void * d) {
    Rack * rack = (Rack *) d ;
    rack -> next_preset () ;
}

void preset_prev (void * b, void * d) {
    Rack * rack = (Rack *) d ;
    rack -> prev_preset ();
}

int get_index (GtkWidget * w) {
    return gtk_widget_get_name ((GtkWidget *) w) [0] - 48 ;    
}

void set_index (GtkWidget * w, int index) {
    char name [3];
    name [0] = index + 48 ;
    name [1] = 0 ;
        
    gtk_widget_set_name (w, name);
}

void test_plugins (void * d) {
    IN
    Rack * rack = (Rack *) d;
    for (auto it=rack -> engine->lv2Json.begin () ; it != rack -> engine -> lv2Json.end () ; it ++) {
    //~ for (auto plugin : engine ->lv2Json) {
        auto plugin = it .value ();
        std::string a = plugin ["name"].dump() ;
        a = a.substr (1, a.size () - 2);
        //~ LOGD ("p: %s\n", it.key ().c_str ());
        
        if (rack -> blacklist.contains (it.key ().c_str ())) {
            continue;
        }
      
        wtf ("\n\n------------| lv2 test %s |-----------------\n", a.c_str ());
        rack -> addPluginByName ((char *) a.c_str ()) ;
        rack -> clear () ;
    }    

    for (auto plugin : rack -> engine ->ladspaJson) {
        std::string a = plugin ["name"].dump() ;
        a = a.substr (1, a.size () - 2);
        std::string id = plugin ["id"].dump () ;
        
        if (rack -> blacklist.contains (id)) {
            //~ LOGD ("blacklisted plugin [%s]: %s\n", id.c_str (), a.c_str ()) ;
            continue;
        }        

        wtf ("\n\n------------| ladspa test %s |-----------------\n", a.c_str ());
        rack -> addPluginByName ((char *) a.c_str ()) ;
        rack -> clear () ;
    }

    OUT ;
}

void do_search (void * w, void *d) {
    Sorter * sorter = (Sorter *) d ;
    Rack * rack = (Rack *) sorter -> rack ;
    GtkEntry * tb = (GtkEntry *) w ;
    const char * text = gtk_entry_buffer_get_text (gtk_entry_get_buffer (tb));
    if (text [0] == '#') {
        auto iter = rack -> hashCommands.find(std::string (++text));
        if (iter == rack->hashCommands.end()) {
            wtf ("[hash command] not found: %s\n", text);
            return ;
        }

        (*iter->second)(rack);
        return ;
    }
    
    std::string str = std::string (text);
    std::transform(str.begin(), str.end(), str.begin(), ::tolower);
    bool all = str.size () == 0;
    
    for (int i = 0 ; i < rack -> hearts.size (); i ++) {
        GtkWidget * w = (GtkWidget * )rack -> hearts.at (i);
        char * wname = (char *)gtk_widget_get_name (w);
        w = gtk_widget_get_parent (w);
        
        if (all) {           
            gtk_widget_set_visible (gtk_widget_get_parent (w), true);
            continue ;
        }
        
        LOGD ("[match] %s -> %s\n", wname, text) ;
        std::string haystack = std::string (wname) ;
        std::transform(haystack.begin(), haystack.end(), haystack.begin(), ::tolower);

        if (haystack.find (str) != -1) 
            gtk_widget_set_visible (gtk_widget_get_parent (w), true);
        else
            gtk_widget_set_visible (gtk_widget_get_parent (w), false);
        
    }
    
}

void show_only_fav_plugins (void * w, void * d) {
    IN
    Sorter * sorter = (Sorter *) d ;
    Rack * rack = (Rack *) sorter -> rack ;
    GtkToggleButton * tb = (GtkToggleButton *) w ;
    
    bool show = !gtk_toggle_button_get_active (tb) ;
    for (int i = 0 ; i < rack -> hearts.size () ; i ++) {
        auto fav = rack -> hearts.at (i);
        gtk_widget_set_visible (gtk_widget_get_parent (sorter -> boxes->at (i)), show || gtk_toggle_button_get_active ((GtkToggleButton *)fav));
    }
    
    OUT
}

void show_only_categories (void * w, int event, void * d) {
    GtkWidget * dropdown = (GtkWidget * ) w ;
    Sorter * sorter = (Sorter *) d;
    
    //~ printf ("select %s\n", gtk_string_object_get_string ((GtkStringObject *)gtk_drop_down_get_selected_item ((GtkDropDown *)dropdown)));    
    char * name = (char *) malloc (20);
    std::vector <int> ps ;

    bool all = false ;
    if (strcmp ((char *) gtk_widget_get_name (dropdown), "categories") == 0) {
        if (strcmp (gtk_string_object_get_string ((GtkStringObject *)gtk_drop_down_get_selected_item ((GtkDropDown *)dropdown)), "All") == 0) {
            all = true ;
        }
        
        ps = sorter -> engine ->categories [gtk_string_object_get_string ((GtkStringObject *)gtk_drop_down_get_selected_item ((GtkDropDown *)dropdown))].get <std::vector<int>>() ;
    }
    else
        ps = sorter -> engine ->creators [gtk_string_object_get_string ((GtkStringObject *)gtk_drop_down_get_selected_item ((GtkDropDown *)dropdown))].get <std::vector<int>>() ;
    
    for (int i = 0 ; i < sorter -> boxes->size (); i ++) {
        GtkWidget * w = (GtkWidget * )sorter->boxes->at (i);
        char * wname = (char *)gtk_widget_get_name (w);
        
        if (all) {           
            gtk_widget_set_visible (gtk_widget_get_parent (w), true);
            continue ;
        }
            
        gtk_widget_set_visible (gtk_widget_get_parent (w), false);
        for (int i = 0 ; i < ps.size();  i++) {
            int plugin = ps.at (i);
            sprintf (name, "%d", plugin);
            if (strcmp (name, wname) == 0) {
                //~ LOGD ("[sorter] matched %s to %s\n", name, wname);
                gtk_widget_set_visible (gtk_widget_get_parent (w), true);
            } 
        }
    }

    
    free (name);
}

void change_sort_by (void * w, int event, void * d) {
    GtkWidget * sortBy = (GtkWidget *) w;
    Sorter * sorter = (Sorter *) d;
    
    if (gtk_drop_down_get_selected ((GtkDropDown *)sortBy) == 0) {
        gtk_widget_set_visible (sorter -> categories, true);
        gtk_widget_set_visible (sorter -> creators, false);
    } else {
        gtk_widget_set_visible (sorter -> categories, false);
        gtk_widget_set_visible (sorter -> creators, true);        
    }    
}

void Rack::move_down (PluginUI * ui) {
    IN
    int index = ui -> index ;
    if (index >= engine -> activePlugins->size ())
        return ;
        
    GtkWidget * lower = (GtkWidget *) ui -> card.gobj () ;
    GtkWidget * upper = gtk_widget_get_next_sibling (lower);
    
    //~ auto it = plugs.begin() + ui -> index;
    //~ std::rotate(it, it - 1, plugs.end());
    
    gtk_box_reorder_child_after (list_box.gobj (), lower, upper);
    LOGD ("before sort ...\n");
    engine -> print ();
    engine -> moveActivePluginDown (index);
    
    //~ build () ;

    set_index (lower, index + 1);
    set_index (upper, index);

    uiv.at (index + 1)-> index -- ;
    ui -> index ++ ;
    LOGD ("[rack] moved %d -> %d \n", index, ui -> index);
    
    engine -> print () ;
    OUT
}

void Rack::move_up (PluginUI * ui) {
    IN
    int index = ui -> index ;
    if (index == 0)
        return ;
    
    GtkWidget * lower = (GtkWidget *) ui -> card.gobj () ;
    GtkWidget * upper = gtk_widget_get_prev_sibling (lower);
    
    //~ auto it = plugs.begin() + ui -> index;
    //~ std::rotate(it, it - 1, plugs.end());
    
    gtk_box_reorder_child_after (list_box.gobj (), upper, lower);
    
    LOGD ("before sort ...\n");
    engine -> print () ;
    engine -> moveActivePluginUp (index);
    
    //~ build () ;
    //~ ui -> index = ui -> get_index () ;

    set_index (lower, index - 1);
    set_index (upper, index);

    uiv.at (index + 1)-> index ++ ;
    ui -> index -- ;
    LOGD ("[rack] moved %d -> %d \n", index, ui -> index);
    engine -> print () ;
    OUT
}

PluginUI * Rack::addPluginByName (char * requested) {
    bool res = false ;
    bool has_file = false ;
    PluginFileType file_type = FILE_AUDIO ;
    
    for (auto plugin : engine ->lv2Json) {
        std::string a = plugin ["name"].dump() ;
        a = a.substr (1, a.size () - 2);
        //~ LOGD ("[lv2] %s | %s\n", requested, a.c_str());
        if (strcmp (a.c_str (), requested) == 0) {
            int index = plugin ["index"].get <int> () ;
            std::string lib = plugin ["library"].dump () ;
            lib = std::string (engine -> libraryPath) + lib.substr (1, lib.size () - 2);
            //~ LOGD ("found plugin %s: loading %s\n", requested, lib.c_str ());
            res = engine ->addPlugin ((char *)lib.c_str (), index, SharedLibrary::PluginType::LV2);
            if (plugin.contains ("file")) {
                has_file = true ;                
                file_type = (PluginFileType) plugin ["fileType"].get <int> ();
                //~ LOGD ("got file type: %d\n", file_type);
            }
            break ;
        }
    }
    
    if (! res) {
        for (auto plugin : engine ->ladspaJson) {
            std::string a = plugin ["name"].dump() ;
            a = a.substr (1, a.size () - 2);
            //~ LOGD ("[ladspa] %s | %s\n", requested, a.c_str());
            if (strcmp (a.c_str (), requested) == 0) {
                std::string lib = plugin ["library"].dump () ;
                //~ LOGD ("[ladspa] found plugin %s: loading %s\n", requested, lib.c_str ());
                int index = plugin ["plugin"].get <int> () ;
                lib = std::string (engine -> libraryPath) + lib.substr (1, lib.size () - 2);
                res = engine ->addPlugin ((char *)lib.c_str (), index, SharedLibrary::PluginType::LADSPA);
                if (plugin.contains ("file") && plugin ["file"].get <bool>()) {
                    has_file = true ;                
                    file_type = (PluginFileType) plugin ["fileType"].get <int> ();
                }
                break ;
            }
        }
    }
    
    //~ return ;
    if (res) {
        int index = engine -> activePlugins->size () - 1;
        if (has_file) {
            engine -> activePlugins -> at (index)->loadedFileType = file_type;
        }
        
        PluginUI * ui = new PluginUI (engine, engine -> activePlugins->at (index), &list_box, std::string ((char *) requested), index, has_file, this);
        ui -> pType = (PluginFileType *) malloc (sizeof (int)) ;
        * ui->pType = file_type ;
        ui -> rack = (void * )this ;
        // ui.index = index ;
        list_box.set_orientation (Gtk::Orientation::VERTICAL);
        list_box.set_vexpand (true);
        list_box.append (ui->card);
        plugs.push_back ((GtkWidget * ) ui->card.gobj ());
        uiv.push_back (ui);
        return ui ;
    } else {
        LOGD ("ERROR: failed to load plugin: %s\n", requested);
        return NULL ;
    }    
}
void addPluginCallback (void * b, void * c) {
    GtkWidget * button = (GtkWidget *) b ;
    Rack * rack = (Rack *) c ;
    Engine * engine = (Engine *) rack -> engine ;
    char * requested = (char *) gtk_widget_get_name (button) ;
    rack -> addPluginByName (requested);
}

GtkWidget * Rack::addPluginEntry (std::string plug) {
        GtkWidget * box = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 10);
        gtk_widget_set_margin_start (box, 10);
        gtk_widget_set_margin_end (box, 10);
        GtkWidget * label = gtk_label_new (plug.c_str ());
        GtkWidget * fav = gtk_toggle_button_new ();
        gtk_button_set_label ((GtkButton *) fav, "♥");
        gtk_widget_set_name (fav, plug.c_str ());
        
        hearts.push_back (fav);
        
        GtkWidget * button = gtk_button_new () ;
        gtk_button_set_label ((GtkButton *) button, "+");
        
        gtk_widget_set_name (button, plug.c_str ());
        
        g_signal_connect (button, "clicked", GCallback (addPluginCallback), this);
        
        gtk_widget_set_hexpand (box, true);
        GtkBox * B = (GtkBox *)gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
        gtk_widget_set_name ((GtkWidget *)B, "plugin");
        gtk_box_append (B, label);
        gtk_widget_set_hexpand (label, false);
        gtk_widget_set_hexpand ((GtkWidget *) B, true);
        //~ gtk_widget_set_halign (label, GTK_ALIGN);
        //~ gtk_label_set_justify ((GtkLabel *) label, GTK_JUSTIFY_LEFT);
        gtk_label_set_wrap ((GtkLabel *) label, true);
        
        gtk_box_append ((GtkBox *)box, (GtkWidget *)B);
        gtk_box_append ((GtkBox *)box, button);
        gtk_box_append ((GtkBox *)box, fav);
        
        gtk_list_box_append ((GtkListBox *)listBox, box);      
        return box ;      
}

void Rack::add () {
    //~ engine -> addPlugin ("libs/dyson_compress_1403.so", 0, SharedLibrary::PluginType::LADSPA);
    engine -> addPlugin ((char *)std::string ("http://drobilla.net/plugins/mda/Delay").c_str (), 0, SharedLibrary::PluginType::LILV);
    int index = engine -> activePlugins->size () - 1;
    PluginUI * ui = new PluginUI (engine, engine -> activePlugins->at (index), & list_box, std::string ("Dyson Compressor"), index, false, this);
    // ui.index = index ;
    list_box.set_orientation (Gtk::Orientation::VERTICAL);
    list_box.set_vexpand (true);
    list_box.append (ui->card);
}

GtkWidget * Rack::createPluginDialog () {
    IN
    pluginDialog = gtk_window_new ();
    gtk_window_set_default_size ((GtkWindow *)pluginDialog, 400, 800);
    GtkWidget * master = gtk_box_new (GTK_ORIENTATION_VERTICAL, 10);
    gtk_widget_set_name (master, "rack");
    //~ gtk_window_set_child ((GtkWindow *)pluginDialog, master);
    
    GtkWidget * chooser = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_box_append ((GtkBox *)master, chooser);
    gtk_widget_set_name (chooser, "rack");
    
    char * category [50] ;
    char * creators [60] ;
    
    int i = 0 ;
    for (auto cat=engine->categories.begin () ; cat != engine -> categories.end () ; cat ++) {
        std::string a = cat.key () ;
        //~ LOGD ("[category] %s\n", a.c_str ());
        category [i] = strdup (a.c_str ());
        i ++ ;
    }
    
    category [i] = NULL;
    
    i = 0 ;
    for (auto cat=engine->creators.begin () ; cat != engine -> creators.end () ; cat ++) {
        std::string a = cat.key () ;
        //~ LOGD ("[creator] %s\n", a.c_str ());
        creators [i] = strdup (a.c_str ());
        i ++ ;
    }

    creators [i] = NULL;
    
    char * ob [] = {
        strdup ("Category"),
        strdup ("Creator"),
        NULL
    } ;
    
    GtkWidget * sortBy = gtk_drop_down_new_from_strings (ob) ;
    GtkWidget * categories = gtk_drop_down_new_from_strings (category);
    gtk_widget_set_name(categories, "categories");
    GtkWidget * creators_w = gtk_drop_down_new_from_strings (creators);
    gtk_widget_set_name (creators_w, "creators");
    
    GtkWidget * show_only_favorites = (GtkWidget *) gtk_toggle_button_new_with_label ("♥");
    gtk_widget_set_hexpand (show_only_favorites, false);
    gtk_box_set_homogeneous ((GtkBox *) chooser, false);
    
    search = gtk_entry_new ();
    gtk_entry_set_placeholder_text ((GtkEntry * ) search, "Search");
    
    GtkWidget * hbox = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_box_append ((GtkBox *)hbox, search);
    gtk_box_append ((GtkBox *)hbox, show_only_favorites);

    gtk_widget_set_hexpand (hbox, false);
    gtk_widget_set_halign (show_only_favorites, GTK_ALIGN_END);
    gtk_widget_set_halign (hbox, GTK_ALIGN_END);
        
    Sorter * sorter = (Sorter *) malloc (sizeof (Sorter));
    sorter -> boxes = new std::vector <GtkWidget *>();
    sorter->categories = categories;
    sorter->creators = creators_w ;
    sorter -> engine = engine ;
    sorter -> listBox = listBox ;
    sorter -> rack = (void *) this ;
    
    gtk_box_append ((GtkBox *)chooser, sortBy);
    gtk_box_append ((GtkBox *)chooser, categories);
    gtk_box_append ((GtkBox *)chooser, creators_w);
    gtk_box_append ((GtkBox *)chooser, hbox);
    
    gtk_box_set_homogeneous ((GtkBox *) chooser, true);
    
    gtk_widget_set_visible (creators_w, false);
    
    g_signal_connect (sortBy, "notify::selected", (GCallback)change_sort_by, sorter);
    g_signal_connect (categories, "notify::selected", (GCallback)show_only_categories, sorter);
    g_signal_connect (creators_w, "notify::selected", (GCallback)show_only_categories, sorter);
    g_signal_connect (show_only_favorites, "toggled", (GCallback)show_only_fav_plugins, sorter);
    g_signal_connect (search, "activate", (GCallback)do_search, sorter);
    
    gtk_widget_set_hexpand (sortBy, true);
    gtk_widget_set_hexpand (categories, true);
    gtk_widget_set_hexpand (chooser, true);
    gtk_widget_set_vexpand (master, true);
    
    GtkWidget * sw = gtk_scrolled_window_new ();
    listBox = gtk_list_box_new ();
    gtk_scrolled_window_set_child ((GtkScrolledWindow *)sw, listBox);
    
    gtk_box_append ((GtkBox *) master, sw);
    
    gtk_widget_set_name ((GtkWidget *)listBox, "rack");

    gtk_widget_set_vexpand (listBox, true);
    gtk_widget_set_hexpand (listBox, true);
    
    char * name = (char *) malloc (100);
    for (auto it=engine->lv2Json.begin () ; it != engine -> lv2Json.end () ; it ++) {
    //~ for (auto plugin : engine ->lv2Json) {
        auto plugin = it .value ();
        std::string a = plugin ["name"].dump() ;
        //~ LOGD ("p: %s\n", it.key ().c_str ());
        
        if (blacklist.contains (it.key ().c_str ())) {
            //~ LOGD ("blacklisted plugin: %s\n", a.c_str ()) ;
            continue;
        }
        
        int id = plugin ["id"].get <int>() ;
        //~ printf ("plugin %d: %s\n", id, a.c_str());
        GtkWidget * w = (GtkWidget *) addPluginEntry (a.substr (1, a.size () - 2));
        sorter -> boxes->push_back (w);
        sprintf (name, "%d", id);
        gtk_widget_set_name (w, name);
    }
   
    for (auto plugin : engine ->ladspaJson) {
        std::string a = plugin ["name"].dump() ;
        std::string id = plugin ["id"].dump () ;
        //~ printf ("[ladspa] plugin %s: %s\n", id.c_str (), a.c_str());
        if (blacklist.contains (id)) {
            //~ LOGD ("blacklisted plugin [%s]: %s\n", id.c_str (), a.c_str ()) ;
            continue;
        }        

        GtkWidget * w = (GtkWidget *) addPluginEntry (a.substr (1, a.size () - 2));
        //~ sprintf (name, "%d", id);

        sorter -> boxes->push_back (w);
        gtk_widget_set_name (w, id.c_str ());
    }
    
    free (name);

    //~ for (int i = 0 ; i < engine -> lv2Plugins->size () ; i ++) {
        //~ addPluginEntry (engine -> lv2Plugins->at (i));
    //~ }
    
    //~ for (int i = 0 ; i < engine -> ladspaPlugins->size () ; i ++) {
        //~ addPluginEntry (engine -> ladspaPlugins->at (i));
    //~ }
       
    //~ gtk_window_present ((GtkWindow *)pluginDialog);
    OUT
    return master ;
}


bool Rack::load_preset (std::string filename) {
    json j = filename_to_json (filename) ;
    if (j != NULL)
        return load_preset (j);
    else 
        return false ;
}

bool Rack::load_preset (json j) {
    IN
    current_patch.set_text (j ["name"].dump ().c_str ());
    auto plugins = j ["controls"];
    clear () ;
    int index = 0 ;
    for (auto p: plugins) {
        auto plugin = p ["name"].dump () ;
        plugin = plugin.substr (1, plugin.size () - 2) ;
        PluginUI * ui = addPluginByName ((char *) plugin.c_str ());
        auto controls = p ["controls"].dump () ;
        controls = controls.substr (1, controls.size () - 2);
        ui -> load_preset (controls) ;
        if (p.contains ("filename")) {
            std::string filename = p ["filename"].dump () ;
            filename = filename.substr (1, filename.size () - 2) ;
            wtf ("[preset] loading file: %s\n", filename.c_str ());
            
            if (p ["filetype"].get <int> () == 0) {
                engine -> set_plugin_audio_file (index, (char *) filename.c_str ());
            } else {
                engine -> set_plugin_file (index, (char *)  filename.c_str ());          
            }
        }
        
        index ++ ;
    }
    
    //~ build () ;
    OUT
    return true;
}

void Rack::clear () {
    IN
    for (int i = 0 ; i < plugs.size () ; i ++) {
        gtk_box_remove (list_box.gobj (), (GtkWidget *)plugs.at (i));
    }
    
    if (engine != NULL && engine->activePlugins != NULL) {
        engine->activePlugins->clear ();
        engine -> buildPluginChain ();
    }
    
    plugs.clear () ;
    OUT
}

void onoff_cb (void * s, bool state, void * d) {
    Engine * e = (Engine *) d ;
    if (!state) {
        e -> driver -> deactivate () ;
    } else {
        e -> driver -> activate () ;
    }
}

void launch_tuner (void * a, void * c) {
    system ("$HOME/bin/xtuner") ;
}

void launch_sync (void * a, void * c) {
    system ("python $HOME/bin/sync.py") ;
}

Rack::Rack () {
    engine = new Engine () ;
    blacklist = filename_to_json ("assets/blacklist.json");
    
    config = filename_to_json (std::string (getenv ("HOME")).append ("/.config/amprack/config.json"));    
    if (config.contains ("theme")) {
        theme = config ["theme"].dump ();
        theme = theme.substr (1, theme.size () - 2);
    } else {
        theme = std::string ("TubeAmp");
    }
    
    //~ for (auto b = blacklist.begin () ; b != blacklist.end () ; b ++) {
        //~ LOGD ("[blacklist] %s\n", b.key ()) ;
    //~ }
    master = Gtk::Box ();
    button_box = Gtk::HeaderBar ();
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
    onoff = Gtk::Switch ();
    //~ onoff.set_label ("On");
    record = Gtk::ToggleButton ();
    record.set_label ("Rec");
    g_signal_connect (record.gobj (), "toggled", (GCallback) toggle_record, engine);
    
    Gtk::Button syn = Gtk::Button ("Sync");
    Gtk::Button tune = Gtk::Button ("Tuner");
    Gtk::ToggleButton toggle_presets = Gtk::Button ("Effects");
    
    g_signal_connect (syn.gobj (), "clicked", (GCallback) launch_sync, NULL);
    g_signal_connect (tune.gobj (), "clicked", (GCallback) launch_tuner, NULL);
    g_signal_connect (toggle_presets.gobj (), "clicked", (GCallback) toggle_effects, this);
    
    patch_up = Gtk::Button ("↑");
    patch_down = Gtk::Button ("↓");
    menu_button.set_margin (5);

    g_signal_connect (patch_up.gobj (), "clicked", (GCallback) preset_next, this);
    g_signal_connect (patch_down.gobj (), "clicked", (GCallback) preset_prev, this);
    
    current_patch = Gtk::Label ("Patch");
    
    logo.set_margin (5);
    mixer_toggle.set_margin (5);
    record.set_margin (5);
    onoff.set_margin (5);

    onoff.set_active (true);
    g_signal_connect (onoff.gobj (), "state-set", (GCallback) onoff_cb, engine);
    
    master.set_orientation (Gtk::Orientation::VERTICAL);
    master.set_vexpand (true);
    list_box.set_vexpand (true);
    master.set_hexpand (true);
    //~ button_box.set_orientation (Gtk::Orientation::HORIZONTAL);

    list_box.set_orientation (Gtk::Orientation::VERTICAL);
    // list_box.set_visible (false);
    //~ master.append (button_box);
    master.append (mixer);
    master.append (sw);
    sw.set_child (list_box);
    list_box.set_name ("rack");
    Gtk::Separator sep = Gtk::Separator () ;
    // list_box.append (sep);
    // sep.set_vexpand (true);
    master.append (add_effect);
    add_effect.set_label ("Clear");
    add_effect.set_name ("delete");
    add_effect.set_valign (Gtk::Align::END);
    // list_box.set_valign (Gtk::Align::END);
    // sw.set_propagate_natural_height (true);
    add_effect.set_halign (Gtk::Align::CENTER);
    
    add_effect.signal_clicked().connect(sigc::mem_fun(*this,
          &Rack::clear));

    //~ button_box.set_halign (Gtk::Align::CENTER);
    //~ button_box.set_hexpand (true);
    
    //~ button_box.pack_start (logo);
    //~ button_box.pack_start (mixer_toggle);
    logo.set_halign (Gtk::Align::START);
    
    Gtk::Box m = Gtk::Box () ;
    m.set_orientation (Gtk::Orientation::VERTICAL);
    
    m.append (onoff);
    
    mixer_toggle.set_halign (Gtk::Align::END);
    
    Gtk::Label l = Gtk::Label ("On") ;
    
    m.append (l);
    //~ button_box.pack_start (m);
    Gtk::Box v = Gtk::Box (Gtk::Orientation::HORIZONTAL, 5) ;
    button_box.set_title_widget (v);
    std::string version = std::string ("<big><b>Amp Rack 5 alpha build ").append (std::to_string (VERSION)).append ("</b></big>");
    Gtk::Label title = Gtk::Label (version.c_str ()) ;
    title.set_markup (version.c_str ());
    title.set_visible (false);
    
    v.append (toggle_presets);
    v.append (syn);
    v.append (tune);
    v.append (patch_up);
    v.append (current_patch);
    v.append (patch_down);

    v.append (title);
    v.append (record) ;
    v.append (m) ;
    
    title.set_margin_end (10);
    
    m.set_halign (Gtk::Align::END);
    v.set_halign (Gtk::Align::END);
    v.set_hexpand (true);
    
    //~ button_box.pack_start (record);
    //~ button_box.pack_start (menu_button);
    
    //~ button_box.set_margin (10);
    
    //~ overlay.set_child (master);
    //~ overlay.add_overlay (add_effect);
    //~ overlay.set_clip_overlay (add_effect, false);
    //~ overlay.set_measure_overlay (add_effect, false);
    
    //~ overlay.set_halign (Gtk::Align::END);
    //~ overlay.set_valign (Gtk::Align::END);
    //~ add_effect.set_label ("+ Effect");

    rack = createPluginDialog () ;
    hashCommands.emplace (std::make_pair (std::string ("test"), &test_plugins));
}

void Rack::build () {
    IN
    int pos = 0 ;
    
    GtkWidget * widget = (GtkWidget *) gtk_widget_get_first_child ((GtkWidget *)list_box.gobj ());
    char name [3] ;
    while (widget != NULL) {
        const char * _name = gtk_widget_get_name (widget) ;
        if (_name == NULL || ! GTK_IS_BOX ( widget))
            continue ;
        
        name [0] = pos++ + 48 ; // funking brilliant
        name [1] = 0 ;
        
        const char * pname = "No plugin" ;
        if (pos < engine -> activePlugins->size ())
            pname = engine -> activePlugins->at (pos)->lv2_name.c_str () ;
        
        printf ("[rack] %s: %s [%d] -> %s\n", pname, _name, pos, name);
        gtk_widget_set_name (widget, name);
        widget = (GtkWidget *) gtk_widget_get_next_sibling (widget) ;
    }
    
    engine -> print ();
    OUT
}

void Rack:: next_preset () {
    Rack * rack = this ;
    Presets * presets = (Presets *) rack -> presets ;
    int which = presets -> presets.get_current_page ();
    
    if (presets -> list_of_presets [which]->size () == 0) {
        wtf ("[patch] no patches in %d\n", which);
        return ;
    }
    
    rack -> patch ++ ;
    if (rack -> patch > presets -> list_of_presets [which]->size () - 1)
        rack -> patch = 0 ;
    
    wtf ("[patch] tab: %d: %d\n", which, rack -> patch);

    rack -> load_preset (presets -> list_of_presets [which]-> at (rack -> patch));
    rack -> current_patch.set_text (presets -> list_of_presets [which]-> at (rack -> patch) ["name"].dump().c_str ());
    
}

void Rack::prev_preset () {
    Rack * rack = this ;
    Presets * presets = (Presets *) rack -> presets ;
    int which =  presets -> presets.get_current_page ();
    if (presets -> list_of_presets [which]->size () == 0) {
        wtf ("[patch] no patches in %d\n", which);
        return ;
    }
    
    rack -> patch -- ;
    if (rack -> patch < 0)
        rack -> patch =  presets -> list_of_presets [which]->size () - 1;
    
    wtf ("[patch] tab: %d: %d\n", which, rack -> patch);
    rack -> load_preset ( presets -> list_of_presets [which]-> at (rack -> patch));
    rack -> current_patch.set_text (presets -> list_of_presets [which]-> at (rack -> patch) ["name"].dump().c_str ());
    
}
