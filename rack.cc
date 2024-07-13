#include "rack.h"

void show_only_categories (void * w, int event, void * d) {
    GtkWidget * dropdown = (GtkWidget * ) w ;
    Sorter * sorter = (Sorter *) d;
    
    printf ("select %s\n", gtk_string_object_get_string ((GtkStringObject *)gtk_drop_down_get_selected_item ((GtkDropDown *)dropdown)));    
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
    
    for (int i = 0 ; i < sorter -> boxes.size (); i ++) {
        GtkWidget * w = (GtkWidget * )sorter->boxes.at (i);
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

GtkWidget * Rack::addPluginEntry (std::string plug) {
        GtkWidget * box = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 10);
        GtkWidget * label = gtk_label_new (plug.c_str ());
        GtkWidget * fav = gtk_toggle_button_new ();
        gtk_button_set_label ((GtkButton *) fav, "♥");
        GtkWidget * button = gtk_button_new () ;
        gtk_button_set_label ((GtkButton *) button, "+");
        
        gtk_widget_set_hexpand (box, true);
        gtk_widget_set_hexpand (label, true);
        //~ gtk_widget_set_halign (label, GTK_ALIGN);
        //~ gtk_label_set_justify ((GtkLabel *) label, GTK_JUSTIFY_LEFT);
        gtk_label_set_wrap ((GtkLabel *) label, true);
        
        gtk_box_append ((GtkBox *)box, label);
        gtk_box_append ((GtkBox *)box, button);
        gtk_box_append ((GtkBox *)box, fav);
        
        gtk_list_box_append ((GtkListBox *)listBox, box);      
        return box ;      
}

void Rack::add () {
    //~ engine -> addPlugin ("libs/dyson_compress_1403.so", 0, SharedLibrary::PluginType::LADSPA);
    engine -> addPlugin ((char *)std::string ("http://drobilla.net/plugins/mda/Delay").c_str (), 0, SharedLibrary::PluginType::LILV);
    int index = engine -> activePlugins->size () - 1;
    PluginUI * ui = new PluginUI (engine, engine -> activePlugins->at (index), & list_box, std::string ("Dyson Compressor"), index);
    // ui.index = index ;
    list_box.set_orientation (Gtk::Orientation::VERTICAL);
    list_box.set_vexpand (true);
    list_box.append (ui->card);
}

void Rack::createPluginDialog () {
    IN
    pluginDialog = gtk_window_new ();
    gtk_window_set_default_size ((GtkWindow *)pluginDialog, 400, 800);
    GtkWidget * master = gtk_box_new (GTK_ORIENTATION_VERTICAL, 10);
    gtk_window_set_child ((GtkWindow *)pluginDialog, master);
    
    GtkWidget * chooser = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_box_append ((GtkBox *)master, chooser);
    
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
        "Category",
        "Creator",
        NULL
    } ;
    
    GtkWidget * sortBy = gtk_drop_down_new_from_strings (ob) ;
    GtkWidget * categories = gtk_drop_down_new_from_strings (category);
    gtk_widget_set_name(categories, "categories");
    GtkWidget * creators_w = gtk_drop_down_new_from_strings (creators);
    gtk_widget_set_name (creators_w, "creators");
    
    Sorter * sorter = (Sorter *) malloc (sizeof (Sorter));
    sorter->categories = categories;
    sorter->creators = creators_w ;
    sorter -> engine = engine ;
    sorter -> listBox = listBox ;
    
    gtk_box_append ((GtkBox *)chooser, sortBy);
    gtk_box_append ((GtkBox *)chooser, categories);
    gtk_box_append ((GtkBox *)chooser, creators_w);
    
    gtk_box_set_homogeneous ((GtkBox *) chooser, true);
    
    gtk_widget_set_visible (creators_w, false);
    
    g_signal_connect (sortBy, "notify::selected", (GCallback)change_sort_by, sorter);
    g_signal_connect (categories, "notify::selected", (GCallback)show_only_categories, sorter);
    g_signal_connect (creators_w, "notify::selected", (GCallback)show_only_categories, sorter);
    
    gtk_widget_set_hexpand (sortBy, true);
    gtk_widget_set_hexpand (categories, true);
    gtk_widget_set_hexpand (chooser, true);
    gtk_widget_set_vexpand (master, true);
    
    GtkWidget * sw = gtk_scrolled_window_new ();
    listBox = gtk_list_box_new ();
    gtk_scrolled_window_set_child ((GtkScrolledWindow *)sw, listBox);
    
    gtk_box_append ((GtkBox *) master, sw);
    
    gtk_widget_set_vexpand (listBox, true);
    gtk_widget_set_hexpand (listBox, true);
    
    char * name = (char *) malloc (100);
    for (auto plugin : engine ->lv2Json) {
        std::string a = plugin ["name"].dump() ;
        int id = plugin ["id"].get <int>() ;
        printf ("plugin %d: %s\n", id, a.c_str());
        GtkWidget * w = (GtkWidget *) addPluginEntry (a.substr (1, a.size () - 2));
        sorter -> boxes.push_back (w);
        sprintf (name, "%d", id);
        gtk_widget_set_name (w, name);
    }
   
    for (auto plugin : engine ->ladspaJson) {
        std::string a = plugin ["name"].dump() ;
        int id = plugin ["id"].get <int>() ;
        printf ("plugin %d: %s\n", id, a.c_str());
        GtkWidget * w = (GtkWidget *) addPluginEntry (a.substr (1, a.size () - 2));
        sprintf (name, "%d", id);
        sorter -> boxes.push_back (w);
        gtk_widget_set_name (w, name);
    }
    
    free (name);

    //~ for (int i = 0 ; i < engine -> lv2Plugins->size () ; i ++) {
        //~ addPluginEntry (engine -> lv2Plugins->at (i));
    //~ }
    
    //~ for (int i = 0 ; i < engine -> ladspaPlugins->size () ; i ++) {
        //~ addPluginEntry (engine -> ladspaPlugins->at (i));
    //~ }
       
    gtk_window_present ((GtkWindow *)pluginDialog);
    OUT
}
