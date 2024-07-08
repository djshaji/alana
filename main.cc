#include "main.h"


int main(int argc, char* argv[])
{
  auto app = Gtk::Application::create("org.acoustixaudio.amprack");

  return app->make_window_and_run<MyWindow>(argc, argv);
}


