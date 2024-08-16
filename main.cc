#include "main.h"


int main(int argc, char* argv[])
{
    auto app = Gtk::Application::create("org.acoustixaudio.amprack");

    GtkCssProvider *cssProvider = gtk_css_provider_new();
    gtk_css_provider_load_from_path(cssProvider, "style.css");
    gtk_style_context_add_provider_for_display (gdk_display_get_default (), (GtkStyleProvider *)cssProvider, GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

    MyWindow window = MyWindow (app->gobj ());
    gtk_widget_add_css_class ((GtkWidget *) window.gobj (), "xwindow");

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

MyWindow::MyWindow(GtkApplication * _app)
{
    app = _app ;
    set_title("Amp Rack 5 (Alpha)");
    set_default_size(1200, 800);
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
    
    pane.set_position (900);

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
    
    CB * cb = new CB () ;
    cb -> engine = rack -> engine ;
    cb -> presets = presets ;
    
    g_signal_connect (presets->add.gobj (), "clicked", (GCallback) save_preset_cb, cb);
    
    pane.set_start_child (presets->master);
    pane.set_end_child (rack->master);
    g_signal_connect (this->gobj (), "close-request", (GCallback) quit, this);
    g_signal_connect (this->gobj (), "show", (GCallback) onshow, this);
 
    presets -> notebook.set_current_page (1); 
    
    GtkEventController *   event_controller = gtk_event_controller_key_new ();
    g_signal_connect_swapped (event_controller, "key-pressed",
                           G_CALLBACK (hotkeys),
                           this);
    gtk_widget_add_controller (GTK_WIDGET (gobj()), event_controller);
}

