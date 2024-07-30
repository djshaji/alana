#include "main.h"


int main(int argc, char* argv[])
{
    auto app = Gtk::Application::create("org.acoustixaudio.amprack");

    MyWindow window = MyWindow (app->gobj ());

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


