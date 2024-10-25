# include "sync.h"

Sync::Sync (Rack * r) {
    rack = r;
    
    window = (GtkWindow *) gtk_window_new ();
    gtk_window_set_default_size (window, 600, 300);
    
    GtkGrid * grid = (GtkGrid *) gtk_grid_new ();    
    gtk_window_set_child (window, (GtkWidget *) grid);
    
    gtk_grid_set_row_spacing (grid, 10);
    gtk_grid_set_column_spacing (grid, 10);
    
    gtk_widget_set_margin_top (GW grid, 10);
    gtk_widget_set_margin_start (GW grid, 10);
    gtk_widget_set_margin_bottom (GW grid, 10);
    gtk_widget_set_margin_end (GW grid, 10);

    GtkLabel * header = (GtkLabel *) gtk_label_new ("Sync Presets");
    gtk_label_set_justify (header, GTK_JUSTIFY_CENTER);
    gtk_label_set_markup (header, "<span weight='bold' size='xx-large'>Sync Amp Rack Presets</span>");
                                        //      col row w h
    gtk_grid_attach (grid, (GtkWidget *) header, 0, 0, 3, 1);    
    
    GtkLabel * desc = (GtkLabel *) gtk_label_new ("Use the following details to sync from another device");
    gtk_grid_attach (grid, (GtkWidget *) desc, 0, 1, 3, 1);       
    
    GtkLabel * ip = (GtkLabel * ) gtk_label_new ("xxx.xxx.xxx.xxx");
    GtkLabel * key = (GtkLabel * ) gtk_label_new ("xxxxxx");
    
    gtk_grid_attach (grid, GW gtk_label_new ("IP:"), 0, 2, 1, 1);
    gtk_grid_attach (grid, GW gtk_label_new ("Port:"), 0, 3, 1, 1);
    gtk_grid_attach (grid, GW ip, 1, 2, 2, 1);
    gtk_grid_attach (grid, GW gtk_label_new ("8080"), 1, 3, 2, 1);
    gtk_grid_attach (grid, GW gtk_label_new ("Key:"), 0, 4, 1, 1);
    gtk_grid_attach (grid, GW key, 1, 4, 2, 1);
    
    gtk_grid_attach (grid, gtk_label_new ("Enter details below to sync from this device"), 0, 5, 3, 1);

    GtkEntry * entry = (GtkEntry *) gtk_entry_new ();
    GtkButton * btn = (GtkButton *) gtk_button_new_with_label ("Sync");
    
    gtk_grid_attach (grid, GW entry, 0, 6, 2, 1);
    gtk_grid_attach (grid, GW btn, 2, 6, 1, 1);
    gtk_grid_attach (grid, gtk_label_new ("IP:PORT:KEY"), 0, 7, 2, 1);

    gtk_window_present (window);
}
