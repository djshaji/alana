#include "main.h"

void activate (GApplication * app, void * v) {
    //~ gtk_application_add_window ((GtkApplication *)app, window -> gobj ());
    GtkCssProvider *cssProvider = gtk_css_provider_new();
    GtkCssProvider *cssProvider2 = gtk_css_provider_new();
    gtk_css_provider_load_from_path(cssProvider, "style.css");

    MyWindow window = MyWindow ((GtkApplication *) app);
    gtk_widget_add_css_class ((GtkWidget *) window.window, "xwindow");

	if ( std::filesystem::exists ("assets/themes/TubeAmp/style.css"))
		gtk_css_provider_load_from_path(cssProvider, std::string ("assets/themes/").append (window.rack -> theme).append ("/style.css").c_str ());
	else
		gtk_css_provider_load_from_path(cssProvider, std::string ("/usr/share/amprack/assets/themes/").append (window.rack -> theme).append ("/style.css").c_str ());
    gtk_style_context_add_provider_for_display (gdk_display_get_default (), (GtkStyleProvider *)cssProvider, GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

    std::string user_css = std::string (getenv ("HOME")).append ("/.config/amprack/style.css").c_str () ;
    if (std::filesystem::exists (user_css))
        gtk_css_provider_load_from_path(cssProvider2, user_css.c_str());
    gtk_style_context_add_provider_for_display (gdk_display_get_default (), (GtkStyleProvider *)cssProvider2, GTK_STYLE_PROVIDER_PRIORITY_USER);

    gtk_window_present ((GtkWindow *)window.window);
}

void position_changed (GtkPaned * pane, void *) {
    printf ("[paned] %s\n", gtk_paned_get_position (pane)) ;
}

int main(int argc, char* argv[])
{
    auto app = gtk_application_new ("org.acoustixaudio.amprack", G_APPLICATION_DEFAULT_FLAGS);

    //~ window.set_title("Gtk4 Demo");
    //~ window.set_default_size(300 , 400);
    //~ g_signal_connect (app, "activate")
    //~ app->signal_activate().connect([&](){
        //~ app->add_window(window);
        //~ window.show ();
    //~ });

    g_signal_connect (app, "activate", (GCallback) activate, NULL);
    return g_application_run ((GApplication *)app, argc ,argv);

  //~ GtkSettings * settings = gtk_settings_get_default ();
  //~ g_object_set (settings, "gtk-xft-dpi", 1.1, NULL);
    //~ return app->make_window_and_run<MyWindow>(argc, argv);
}

MyWindow * global_window_really_question_mark = nullptr;

void toggle_effects (GtkToggleButton * button, MyWindow * window) {
    GtkPaned * pane = window -> pane;
    
    if (gtk_toggle_button_get_active (button)) {
        gtk_paned_set_position (pane, 1600) ;
    } else {
        gtk_paned_set_position (pane, 1100) ;
    }

    /*
    GtkAdjustment * h = gtk_scrolled_window_get_hadjustment ((GtkScrolledWindow *) window -> rack -> sw.gobj ());
    GtkAdjustment * v = gtk_scrolled_window_get_vadjustment ((GtkScrolledWindow *) window -> rack -> sw.gobj ());
    printf ("[scroll] %d %d %d | %d %d %d\n",
        gtk_adjustment_get_lower (h),
        gtk_adjustment_get_upper (h),
        gtk_adjustment_get_value (h),
        gtk_adjustment_get_lower (v),
        gtk_adjustment_get_upper (v),
        gtk_adjustment_get_value (v));
    */
}

MyWindow::MyWindow(GtkApplication * _app)
{
    app = _app ;
    window = (GtkWindow *) gtk_application_window_new (app);
    gtk_window_set_title(window, "Amp Rack 5 (Alpha)");
    gtk_window_set_default_size(window, 700, 400);
    gtk_window_maximize (window);

    box = (GtkBox *) gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
    gtk_window_set_child (gobj (), (GtkWidget *)box);

    pane = (GtkPaned *)gtk_paned_new (GTK_ORIENTATION_HORIZONTAL);
    
    stack_box = (GtkBox *)gtk_box_new (GTK_ORIENTATION_VERTICAL, 10);
    
    gtk_box_append (box, (GtkWidget *)stack_box);
    
    gtk_paned_set_position (pane, 800);
    //~ g_signal_connect (pane.gobj (), "notify::position", (GCallback) position_changed, NULL);

    stack = (GtkStack *)gtk_stack_new ();
    gtk_box_append (stack_box, (GtkWidget *) stack);
    gtk_box_set_homogeneous (stack_box, false);

    switcher = (GtkStackSwitcher *)gtk_stack_switcher_new ();
    gtk_box_append (stack_box, (GtkWidget *)switcher);

    rack = new Rack () ;
    gtk_stack_add_child (stack, (GtkWidget *)pane);
    
    gtk_window_set_titlebar (gobj (), (GtkWidget *)rack->button_box);
    
    presets = new Presets () ;
    presets->_this = (void *) presets ;
    presets->engine = rack -> engine ;
    presets->rack = rack ;
    presets->app = app ;
    
    rack -> presets = (void *) presets;
    
    gtk_notebook_append_page (presets->notebook, rack->rack, gtk_label_new ("Effects"));
    
    presets->my () ;
    
    Settings settings = Settings (rack);
    gtk_notebook_append_page (presets->notebook, (GtkWidget *)settings . grid, gtk_label_new ("Settings"));
    
    
    CB * cb = new CB () ;
    cb -> engine = rack -> engine ;
    cb -> presets = presets ;
    
    g_signal_connect (presets->add, "clicked", (GCallback) save_preset_cb, cb);
    
    GtkScrolledWindow * sw = (GtkScrolledWindow *) gtk_scrolled_window_new ();
    gtk_scrolled_window_set_child (sw, (GtkWidget *)presets->master);
    
    gtk_paned_set_start_child (pane, (GtkWidget *)sw);
    gtk_paned_set_end_child (pane, (GtkWidget *)rack->master);
    g_signal_connect (this->gobj (), "close-request", (GCallback) quit, this);
    g_signal_connect (this->gobj (), "show", (GCallback) onshow, this);
 
    gtk_notebook_set_current_page (presets->notebook, 1); 
    
    GtkEventController *   event_controller = gtk_event_controller_key_new ();
    g_signal_connect_swapped (event_controller, "key-pressed",
                           G_CALLBACK (hotkeys),
                           this);
    gtk_widget_add_controller (GTK_WIDGET (gobj()), event_controller);
    
    rack -> hashCommands.emplace (std::make_pair (std::string ("quit"), &qquit));
    //~ g_signal_connect (rack -> toggle_presets.gobj (), "clicked", (GCallback) toggle_effects, this);
    //~ XWarpPointer(gdk_x11_display_get_xdisplay (gdk_display_get_default ()),0,0, 0, 0, 0, 0, 0, 
                //~ 0);

}

GtkWindow * MyWindow::gobj () {
    return window ;
}
