#include "main.h"

void position_changed (GtkPaned * pane, void *) {
    printf ("[paned] %s\n", gtk_paned_get_position (pane)) ;
}

int main(int argc, char* argv[])
{
    auto app = Gtk::Application::create("org.acoustixaudio.amprack");

    GtkCssProvider *cssProvider = gtk_css_provider_new();
    GtkCssProvider *cssProvider2 = gtk_css_provider_new();
    gtk_css_provider_load_from_path(cssProvider, "style.css");

    MyWindow window = MyWindow (app->gobj ());
    gtk_widget_add_css_class ((GtkWidget *) window.gobj (), "xwindow");

	if ( std::filesystem::exists ("assets/themes/TubeAmp/style.css"))
		gtk_css_provider_load_from_path(cssProvider, std::string ("assets/themes/").append (window.rack -> theme).append ("/style.css").c_str ());
	else
		gtk_css_provider_load_from_path(cssProvider, std::string ("/usr/share/amprack/assets/themes/").append (window.rack -> theme).append ("/style.css").c_str ());
    gtk_style_context_add_provider_for_display (gdk_display_get_default (), (GtkStyleProvider *)cssProvider, GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

    std::string user_css = std::string (getenv ("HOME")).append ("/.config/amprack/style.css").c_str () ;
    if (std::filesystem::exists (user_css))
        gtk_css_provider_load_from_path(cssProvider2, user_css.c_str());
    gtk_style_context_add_provider_for_display (gdk_display_get_default (), (GtkStyleProvider *)cssProvider2, GTK_STYLE_PROVIDER_PRIORITY_USER);

    //~ window.set_title("Gtk4 Demo");
    //~ window.set_default_size(300 , 400);
    app->signal_activate().connect([&](){
        app->add_window(window);
        window.show ();
    });

    return app->run(argc , argv);

  //~ GtkSettings * settings = gtk_settings_get_default ();
  //~ g_object_set (settings, "gtk-xft-dpi", 1.1, NULL);
    //~ return app->make_window_and_run<MyWindow>(argc, argv);
}

MyWindow * global_window_really_question_mark = nullptr;

void toggle_effects (GtkToggleButton * button, MyWindow * window) {
    GtkPaned * pane = window -> pane.gobj();
    
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
    set_title("Amp Rack 5 (Alpha)");
    set_default_size(700, 400);
    maximize ();

    box = Gtk::Box () ;
    set_child (box);

    pane = Gtk::Paned () ;
    box.set_orientation (Gtk::Orientation::VERTICAL);
    pane.set_orientation (Gtk::Orientation::HORIZONTAL);

    stack_box = Gtk::Box () ;
    stack_box.set_spacing (10);
    box.append (stack_box);
    stack_box.set_orientation (Gtk::Orientation::VERTICAL);
    
    pane.set_position (370);
    pane.set_position (800);
    //~ g_signal_connect (pane.gobj (), "notify::position", (GCallback) position_changed, NULL);

    stack = Gtk::Stack () ;
    stack_box.append (stack);
    stack_box.set_homogeneous (false);

    switcher = Gtk::StackSwitcher () ;
    stack_box.append (switcher);

    rack = new Rack () ;
    stack.add (pane);
    
    set_titlebar (rack->button_box);
    
    presets = new Presets () ;
    presets->_this = (void *) presets ;
    presets->engine = rack -> engine ;
    presets->rack = rack ;
    presets->app = app ;
    
    rack -> presets = (void *) presets;
    
    gtk_notebook_append_page (presets->notebook.gobj (), rack->rack, gtk_label_new ("Effects"));
    
    presets->my () ;
    
    Settings settings = Settings (rack);
    gtk_notebook_append_page (presets->notebook.gobj (), (GtkWidget *)settings . grid, gtk_label_new ("Settings"));
    
    
    CB * cb = new CB () ;
    cb -> engine = rack -> engine ;
    cb -> presets = presets ;
    
    g_signal_connect (presets->add.gobj (), "clicked", (GCallback) save_preset_cb, cb);
    
    Gtk::ScrolledWindow sw = Gtk::ScrolledWindow () ;
    gtk_scrolled_window_set_child (sw.gobj (), (GtkWidget *)presets->master.gobj ());
    
    pane.set_start_child (sw);
    pane.set_end_child (rack->master);
    g_signal_connect (this->gobj (), "close-request", (GCallback) quit, this);
    g_signal_connect (this->gobj (), "show", (GCallback) onshow, this);
 
    presets -> notebook.set_current_page (1); 
    
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

