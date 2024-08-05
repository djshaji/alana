#include "pluginui.h"

void callback (void * p, void *c) {
  GtkButton * b = (GtkButton *) p ;
  CallbackData *cd = (CallbackData *) c ;

  cd -> parent -> remove (*cd -> card);
  cd -> engine -> activePlugins -> erase(cd -> engine->activePlugins->begin() + cd -> index);
  cd->engine->buildPluginChain();
  // printf ("%s\n", cd -> card -> get_name  ());
}

void pu_move_up (void * b, void * d) {
    PluginUI * ui = (PluginUI *) d ;
    Rack * rack = (Rack *) ui -> rack ;
    LOGD ("up: %d\n", ui -> index);
    rack -> move_up (ui);
}

void pu_move_down (void * b, void * d) {
    PluginUI * ui = (PluginUI *) d ;
    Rack * rack = (Rack *) ui -> rack ;
    rack -> move_down (ui);    
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
    PluginUI * ui = (PluginUI *) cd -> ui;
    
    float val = gtk_adjustment_get_value (adj) ;
    wtf ("[%s] %d : %d-> %f\n", ui -> name.get_text ().c_str (), ui -> get_index (), cd -> control, val);
    if (cd->dropdown != nullptr) {
        gtk_drop_down_set_selected ((GtkDropDown *)cd -> dropdown, val);
    }
    
    *cd -> engine -> activePlugins -> at ( ui -> get_index ())
        -> pluginControls . at (cd -> control)->def = val;
    //~ cd -> engine -> activePlugins -> at (cd -> index) -> print ();
    OUT
}

void PluginUI::remove ()  {
  LOGD ("plugin: %d\n", index) ;
  printf ("delete\n\n");
}

void PluginUI::load_preset (std::string controls) {
    IN
    //~ LOGV (controls.c_str ());
    int i = 0 ;
    std::string c ;
    std::istringstream str = std::istringstream(controls);
    
    while (std::getline(str, c, ';') && i < sliders.size ()) {
        float f = atof(c.c_str());
        //~ LOGD ("[%d] (%s) %f\n", i, c.c_str (), f);
        GtkScale * scale = (GtkScale *)sliders.at (i);
        gtk_range_set_value ((GtkRange *) scale, f);
        i ++;
    }
    
    OUT
}

void on_response (GtkNativeDialog *native,
             int              response, gpointer data)
{
  if (response == GTK_RESPONSE_ACCEPT)
    {
      GtkFileChooser *chooser = GTK_FILE_CHOOSER (native);
      GFile *file = gtk_file_chooser_get_file (chooser);
      PluginUI * ui = (PluginUI *) data;
      char * filename = g_file_get_path (file);
      LOGV (filename);
      LOGD ("requested file type : %d\n", * ui -> pType) ;
      if ( *ui -> pType == 0) {
          LOGD ("[response] %d -> %s\n", * ui -> index_p, filename);
          ui -> engine ->set_plugin_audio_file (* ui -> index_p, filename);
      } else {
          ui -> engine ->set_plugin_file (ui -> index, filename);          
      }
      
      free (filename);
      g_object_unref (file);
    }

  g_object_unref (native);
}

void ui_file_chooser (void * b, void * d) {
  GtkFileChooserNative *native;
  GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_OPEN;

  native = gtk_file_chooser_native_new ("Open File",
                                        NULL,
                                        action,
                                        "_Open",
                                        "_Cancel");

  g_signal_connect (native, "response", G_CALLBACK (on_response), d);
  gtk_native_dialog_show (GTK_NATIVE_DIALOG (native));


}

void dropdown_activated (void * d, int event, void * s) {
    gtk_adjustment_set_value ((GtkAdjustment *) s, gtk_drop_down_get_selected ((GtkDropDown * )d));
}

int PluginUI::get_index () {
    //~ return index ;
    if (! gtk_widget_get_realized (card_))
        return index ;
    return gtk_widget_get_name ((GtkWidget *) card.gobj ()) [0] - 48 ;
}

