#ifndef SETTINGS_H
#define SETTINGS_H

#include <gtkmm.h>
#include <gtk/gtk.h>
#include "rack.h"

class Settings : public Gtk::Grid {
    public:
    Settings (Rack *) ;
};


#endif // SETTINGS_H
