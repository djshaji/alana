#ifndef MAIN_H
#define MAIN_H
#include <filesystem>

//~ #include <gtkmm.h>
#include <gtk/gtk.h>
#include "log.h"
#include "rack.h"
#include "presets.h"
#include "settings.h"

using json = nlohmann::json;

class CB {
    public:
        Presets * presets ;
        Engine * engine ;
};


class MyWindow
{
public:
    GtkWindow * window ;
    GtkPaned * pane ; 
    GtkStack * stack ;
    GtkStackSwitcher * switcher ;
    GtkBox * box, * stack_box ;
    GtkApplication * app ;
    Rack * rack ;
    Presets * presets ; 
    GtkWindow * gobj ();
  MyWindow (GtkApplication *);
};

#endif

