#ifndef CALLBACK_DATA_H
#define CALLBACK_DATA_H

#include <gtkmm.h>
#include "engine.h"

typedef struct {
    Gtk::Widget * card ;
    Gtk::Box * parent ;
    int index ;
    int control ;
    Engine * engine ;
} CallbackData ;

#endif
