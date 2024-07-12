#include "pluginui.h"

void callback (void * p, void *c) {
  GtkButton * b = (GtkButton *) p ;
  CallbackData *cd = (CallbackData *) c ;

  cd -> parent -> remove (*cd -> card);
  cd -> engine -> activePlugins -> erase(cd -> engine->activePlugins->begin() + cd -> index);
  cd->engine->buildPluginChain();
  // printf ("%s\n", cd -> card -> get_name  ());
}

void bypass (void * p, void * c) {
    IN
    GtkToggleButton * t = (GtkToggleButton * ) p;
    CallbackData * cd = (CallbackData *) c ;
    cd -> engine -> activePlugins -> at (cd -> index)-> active = gtk_toggle_button_get_active (t);
    cd -> engine -> buildPluginChain () ;
    OUT
}

void control_changed ( void * p, void * c) {
    IN
    GtkScale * scale = (GtkScale *) p ;
    GtkAdjustment * adj = (GtkAdjustment *) gtk_range_get_adjustment ((GtkRange *)scale);
    CallbackData *cd = (CallbackData *) c ;
    
    *cd -> engine -> activePlugins -> at (cd -> index)
        -> pluginControls . at (cd -> control)->def = gtk_adjustment_get_value (adj);
    cd -> engine -> activePlugins -> at (cd -> index) -> print ();
    OUT
}

void PluginUI::remove ()  {
  LOGD ("plugin: %d\n", index) ;
  printf ("delete\n\n");
}
