#ifndef CALLBACK_DATA_H
#define CALLBACK_DATA_H

#include <gtkmm.h>
#include "engine.h"

typedef struct {
    GtkBox * card ;
    GtkBox * parent ;
    GtkWidget * dropdown ;
    int index ;
    int control ;
    Engine * engine ;
    void * ui ;
} CallbackData ;

#endif
