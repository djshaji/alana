# include "sync.h"

# ifdef __linux__
# include "net.cc"
#endif

void sync_send (Sync * sync) {
    IN
    std::string where = std::string (gtk_entry_buffer_get_text (gtk_entry_get_buffer ((sync -> entry))));
    if (where.size () == 0) {
        where = std::string ("localhost:6906");
    }
    
    int find1 = where.find (":") ;
    int find2 = where.find (":", find1 + 1) - find1;
    
    std::string ip = where.substr (0, find1);
    std::string port = where.substr (find1 + 1, find2 - 1);
    std::string key = where.substr (find2 + find1 + 1);
    Client * client = new Client (ip, std::stoi (port));
    client -> create ();
    Presets * p = (Presets *) sync -> rack -> presets ;
    json j =  p -> get_all_user_presets ();
    //~ j ["key"] = std::stoi (key);
    //~ j ["end"] = {"end: end of input"};
    std::string response = client -> send_preset (j);
    LOGD ("[client] response: %s\n", response.c_str ());
    LOGD ("ip: %s, port: %s, key: %s\n", ip.c_str(), port.c_str (), key.c_str ());

    /// ayyo, importing what we already have!
    if (response.size () > 0) {
        while (response.find ("}}}") != std::string::npos)
            response.pop_back ();
            
        j = json::parse (response);
        int how_many = p -> import_presets_from_json (j);
        char * ss = g_strdup_printf ("<span foreground=\"green\" weight=\"bold\" size=\"x-large\">Imported %d presets successfully</span>", how_many);
        gtk_label_set_markup (sync -> header, ss);
        g_free (ss);
    } else {
        LOGD ("[client] server returned empty response\n");
    }    
    //~ LOGD ("[client] synced %d presets\n", how_many);
    client -> close_socket ();
    OUT
}

void run_server (Sync * sync) {
    IN
    sync -> server = new Server();
    sync -> server -> sync = sync ;
    
    # ifndef __linux__
        sync -> server -> status = sync -> header ;
    # endif 
    
    sync -> server -> presets = (Presets *) sync -> rack -> presets ;
    sync -> server->run();

    OUT
}

void close_sync (Sync * sync) {
    IN
    sync -> server -> close_socket ();
    sync -> t -> join () ;
    gtk_window_destroy (sync -> window);
    # ifndef __linux__
        Presets * p = (Presets *) sync -> rack -> presets ;
        p -> load_user (false);
    # endif
    OUT
}

Sync::Sync (Rack * r) {
    IN
    port = 6906 ;
    rack = r;
    
    window = (GtkWindow *) gtk_window_new ();
    g_signal_connect_swapped (GW window, "close-request", (GCallback)close_sync, this);
    gtk_window_set_default_size (window, 600, 300);
    
    GtkGrid * grid = (GtkGrid *) gtk_grid_new ();    
    gtk_window_set_child (window, (GtkWidget *) grid);
    
    gtk_grid_set_row_spacing (grid, 10);
    gtk_grid_set_column_spacing (grid, 10);
    
    gtk_widget_set_margin_top (GW grid, 10);
    gtk_widget_set_margin_start (GW grid, 10);
    gtk_widget_set_margin_bottom (GW grid, 10);
    gtk_widget_set_margin_end (GW grid, 10);

    header = (GtkLabel *) gtk_label_new ("Sync Presets");
    gtk_label_set_justify (header, GTK_JUSTIFY_CENTER);
    gtk_label_set_markup (header, "<span weight='bold' size='xx-large'>Sync Amp Rack Presets</span>");
                                        //      col row w h
    gtk_grid_attach (grid, (GtkWidget *) header, 0, 0, 3, 1);    
    
    GtkLabel * desc = (GtkLabel *) gtk_label_new ("Use the following details to sync from another device");
    gtk_grid_attach (grid, (GtkWidget *) desc, 0, 1, 3, 1);       
    
    NET net = NET ();
    std::string addresses ;
    for (int i= 0 ; i < net.addresses.size () ; i ++) {
        addresses.append (net.addresses.at (i));
        addresses.append (" ");
    }

    std::random_device rd; // obtain a random number from hardware
    std::mt19937 gen(rd()); // seed the generator
    std::uniform_int_distribution<> distr(1000, 9999); // define the range

    sec_key = distr(gen) ;
    
    GtkLabel * ip = (GtkLabel * ) gtk_label_new (addresses.c_str ());
    GtkLabel * key = (GtkLabel * ) gtk_label_new (g_strdup_printf ("%d", sec_key));
    
    gtk_grid_attach (grid, GW gtk_label_new ("IP:"), 0, 2, 1, 1);
    gtk_grid_attach (grid, GW gtk_label_new ("Port:"), 0, 3, 1, 1);
    gtk_grid_attach (grid, GW ip, 1, 2, 2, 1);
    gtk_grid_attach (grid, GW gtk_label_new (g_strdup_printf ("%d", port)), 1, 3, 2, 1);
    //~ gtk_grid_attach (grid, GW gtk_label_new ("Key:"), 0, 4, 1, 1);
    //~ gtk_grid_attach (grid, GW key, 1, 4, 2, 1);
    
    gtk_grid_attach (grid, gtk_label_new ("Enter details below to sync from this device"), 0, 5, 3, 1);

    entry = (GtkEntry *) gtk_entry_new ();
    GtkButton * btn = (GtkButton *) gtk_button_new_with_label ("Sync");
    
    g_signal_connect_swapped (btn, "clicked", (GCallback) sync_send, this);
    
    status = (GtkLabel *) gtk_label_new ("IP:PORT") ;
    
    gtk_grid_attach (grid, GW entry, 0, 6, 2, 1);
    gtk_grid_attach (grid, GW btn, 2, 6, 1, 1);
    gtk_grid_attach (grid, GW status, 0, 7, 2, 1);

    gtk_window_present (window);
    
    //~ Server server = Server();
    //~ server.run();
    t = new std::thread (&run_server, this);
    //~ t.join ();
    OUT
}
