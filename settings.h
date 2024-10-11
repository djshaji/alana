#ifndef SETTINGS_H
#define SETTINGS_H

#include <gtk/gtk.h>
#include "rack.h"

class Settings {
    public:
    GtkWidget * grid ;
    Settings (Rack *) ;
};


#endif // SETTINGS_H
