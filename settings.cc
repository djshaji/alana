#include "settings.h"

void switch_theme (GtkDropDown * dropdown, int event, Rack * rack) {
	GtkCssProvider *cssProvider = gtk_css_provider_new();
	const char * basename = gtk_string_object_get_string ((GtkStringObject *)gtk_drop_down_get_selected_item ((GtkDropDown *)dropdown));
	if (basename == NULL)
		return ;
	
	std::string filename = std::string ("/usr/share/amprack/assets/themes/").append (std::string (basename)).append ("/style.css");
	if (! std::filesystem::exists (filename))
		return ;
    
    gtk_css_provider_load_from_path(cssProvider, filename.c_str ());
    gtk_style_context_add_provider_for_display (gdk_display_get_default (), (GtkStyleProvider *)cssProvider, GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    rack -> config ["theme"] = std::string (basename);
}

Settings::Settings (Rack * rack) {
	set_name ("plugin");
	GtkLabel * l1 = (GtkLabel *)gtk_label_new ("Theme");
	char * themes [5] = {
		"TubeAmp",
		"Classic",
		"Modern",
		"Analog",
		nullptr
	} ;
	
	GtkDropDown * theme = (GtkDropDown *)gtk_drop_down_new_from_strings (themes);
	gtk_widget_set_margin_end ((GtkWidget *) l1, 10);
	
	g_signal_connect (theme, "notify::selected", (GCallback) switch_theme, rack);
	
	gtk_grid_attach (gobj (), (GtkWidget *)l1, 0, 0, 1, 1);
	gtk_grid_attach (gobj (), (GtkWidget *)theme, 1, 0, 1, 1);
}
