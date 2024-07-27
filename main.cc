#include "main.h"


int main(int argc, char* argv[])
{
  auto app = Gtk::Application::create("org.acoustixaudio.amprack");

  //~ GtkSettings * settings = gtk_settings_get_default ();
  //~ g_object_set (settings, "gtk-xft-dpi", 1.1, NULL);
  return app->make_window_and_run<MyWindow>(argc, argv);
}


