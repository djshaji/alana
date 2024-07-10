#include "pluginui.h"

void callback (void * p, void *c) {
  GtkButton * b = (GtkButton *) p ;
  CallbackData *cd = (CallbackData *) c ;

  cd -> parent -> remove (*cd -> card);
  // printf ("%s\n", cd -> card -> get_name  ());
}

void PluginUI::remove ()  {
  LOGD ("plugin: %d\n", index) ;
  printf ("delete\n\n");
}