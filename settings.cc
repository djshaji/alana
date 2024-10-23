#include "settings.h"

void switch_renderer (GtkDropDown * dropdown, int event, Rack * rack) {
	rack->config ["renderer"] = gtk_drop_down_get_selected (dropdown);
    # ifdef __linux__
    json_to_filename (rack->config, std::string (getenv ("HOME")).append ("/.config/amprack/config.json"));    
    # else
    json_to_filename (rack->config, std::string (getenv ("USERPROFILE")).append ("/.config/amprack/config.json"));    
    # endif
    
}

void switch_theme (GtkDropDown * dropdown, int event, Rack * rack) {
	GtkCssProvider *cssProvider = gtk_css_provider_new();
	const char * basename = gtk_string_object_get_string ((GtkStringObject *)gtk_drop_down_get_selected_item ((GtkDropDown *)dropdown));
	if (basename == NULL)
		return ;
	
	std::string filename = std::string ("assets/themes/").append (std::string (basename)).append ("/style.css");
	if (! std::filesystem::exists (filename))
		return ;
    
    gtk_css_provider_load_from_path(cssProvider, filename.c_str ());
    gtk_style_context_add_provider_for_display (gdk_display_get_default (), (GtkStyleProvider *)cssProvider, GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    rack -> config ["theme"] = std::string (basename);
    # ifdef __linux__
    json_to_filename (rack->config, std::string (getenv ("HOME")).append ("/.config/amprack/config.json"));    
    # else
    json_to_filename (rack->config, std::string (getenv ("USERPROFILE")).append ("/.config/amprack/config.json"));    
    # endif
    
}

Settings::Settings (Rack * rack) {
	grid = gtk_grid_new () ;
	gtk_widget_set_name ((GtkWidget *) grid, "plugin");
	GtkLabel * l1 = (GtkLabel *)gtk_label_new ("Theme");
	GtkLabel * l2 = (GtkLabel *)gtk_label_new ("Renderer");
	const char * themes [5] = {
		"TubeAmp",
		"Classic",
		"Modern",
		"Analog",
		nullptr
	} ;
	
	const char * renderers [7] = {
		"auto",
		"cairo",
		"gl",
		"ngl",
		"vulkan",
		nullptr
	} ;
	
	int current_rend = 0 ;
	if (rack -> config.contains ("renderer")) {
		current_rend = rack -> config ["renderer"].get <int> () ;
		LOGD ("[config] current renderer: %d\n", current_rend);
	}
	
	GtkDropDown * theme = (GtkDropDown *)gtk_drop_down_new_from_strings (themes);
	gtk_widget_set_margin_end ((GtkWidget *) l1, 10);
	
	g_signal_connect (theme, "notify::selected", (GCallback) switch_theme, rack);
	
	gtk_grid_attach ((GtkGrid *) grid, (GtkWidget *)l1, 0, 0, 1, 1);
	gtk_grid_attach ((GtkGrid *) grid, (GtkWidget *)theme, 1, 0, 1, 1);

	GtkDropDown * rend = (GtkDropDown *)gtk_drop_down_new_from_strings (renderers);
	gtk_widget_set_margin_end ((GtkWidget *) l2, 10);
	
	g_signal_connect (rend, "notify::selected", (GCallback) switch_renderer, rack);
	
	gtk_grid_attach ((GtkGrid *) grid, (GtkWidget *)l2, 0, 2, 1, 1);
	gtk_grid_attach ((GtkGrid *) grid, (GtkWidget *)rend, 1, 2, 1, 1);
	
	gtk_drop_down_set_selected (rend, current_rend);
}
