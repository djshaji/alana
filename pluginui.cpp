#include "pluginui.h"

int PluginUI:: index ;

void PluginUI::remove ()  {
  LOGD ("plugin: %d\n", index) ;
  printf ("delete\n\n");
}