#ifndef CALLBACK_DATA_H
#define CALLBACK_DATA_H

#include <gtkmm.h>

typedef struct {
    Gtk::Widget * card ;
    Gtk::Box * parent ;
    int index ;
} CallbackData ;

#endif