#include "pluginui.h"

void callback (void * p, void *c) {
  GtkButton * b = (GtkButton *) p ;
  CallbackData *cd = (CallbackData *) c ;

  cd -> parent -> remove (*cd -> card);
  cd -> engine -> activePlugins -> erase(cd -> engine->activePlugins->begin() + cd -> index);
  cd->engine->buildPluginChain();
  // printf ("%s\n", cd -> card -> get_name  ());
}

void bypass (void * p, bool value, void * c) {
    IN
    GtkToggleButton * t = (GtkToggleButton * ) p;
    CallbackData * cd = (CallbackData *) c ;
    cd -> engine -> activePlugins -> at (cd -> index)-> active = value;
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

void PluginUI::load_preset (std::string controls) {
    IN
    LOGV (controls.c_str ());
    int i = 0 ;
    std::string c ;
    std::istringstream str = std::istringstream(controls);
    
    while (std::getline(str, c, ';') && i < sliders.size ()) {
        float f = atof(c.c_str());
        LOGD ("[%d] (%s) %f\n", i, c.c_str (), f);
        GtkScale * scale = (GtkScale *)sliders.at (i);
        gtk_range_set_value ((GtkRange *) scale, f);
        i ++;
    }
    
    OUT
}
