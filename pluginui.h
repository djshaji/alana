#ifndef PLUGINUI_H
#define PLUGINUI_H

#include <gtkmm.h>
#include <gtk/gtk.h>
#include <vector>
#include <iostream>

#include "json.hpp"
#include "log.h"
#include "defs.h"
#include "engine.h"
#include "Plugin.h"
#include "PluginControl.h"
#include "callback_data.h"

void callback (void * p, void *c);

class PluginUI {
public:    
    Gtk::Label name ;
    Gtk::ToggleButton  onoff ;
    Gtk::Button  del,  up,  down ;
    Engine * engine ;
    Plugin * plugin ;
    Gtk::Box  card ;
    Gtk::Box * parent ;
    int index ;
  
    PluginUI (Engine * _engine, Plugin * _plugin, Gtk::Box * _parent, std::string pluginName, int _index) {
        engine = _engine ;
        plugin = _plugin ;
        index = _index ;
        parent = _parent ;

        char * s = (char *) malloc (pluginName.size () + 3) ;
        sprintf (s, "%d %s", index, pluginName.c_str ());
        // name =  Gtk::Label (s) ;
        name = Gtk::Label (pluginName);
        free (s);
        onoff =  Gtk::ToggleButton ();
        card =  Gtk::Box (Gtk::Orientation::VERTICAL, 0);
        card.set_orientation (Gtk::Orientation::VERTICAL);
        Gtk::Box header = Gtk::Box (Gtk::Orientation::HORIZONTAL, 10) ;
        card.append (header);
        header.set_hexpand (true);
        name.set_hexpand (true);
        name.set_justify (Gtk::Justification::LEFT);
        header.append (name);
        header.set_margin (10);
        header.set_margin_start (0);

        onoff = Gtk::ToggleButton ("On") ;
        header.append (onoff) ;
        onoff.set_halign (Gtk::Align::END);
        name.set_halign (Gtk::Align::START);

        card.set_margin (10);
        del = Gtk::Button ("Delete") ;

        del.set_halign (Gtk::Align::END);
        del.set_valign (Gtk::Align::END);
        del.set_margin (10);

        // del.signal_clicked().connect(sigc::ptr_fun(&callback));
        CallbackData * cd = (CallbackData *) malloc (sizeof (CallbackData));
        cd->index = index ;
        cd -> card = & card ;
        cd -> parent = parent ;

        card.set_name ("hello");

        g_signal_connect (del.gobj (), "clicked", (GCallback) callback, cd);

        for (int i = 0 ; i < plugin->pluginControls.size () ; i ++) {
            PluginControl * control = plugin->pluginControls.at (i) ;
            Gtk::Scale  scale =  Gtk::Scale () ;
            scale . set_value (control->val);
            scale.set_hexpand (true);

            // Gtk::Adjustment  adj =  Gtk::Adjustment (control->val, control->min, control->max, 1, 1, 1);
            Gtk::SpinButton  spin =  Gtk::SpinButton ();

            Gtk::Label  label =  Gtk::Label ();
            LOGD ("control name: %s\n", control->lv2_name.c_str());
            if (plugin->type == SharedLibrary::PluginType::LILV) {
                label.set_label (control->lv2_name.c_str ());
            } else {
                label.set_label (control->name);
            }
            
            Gtk::Box  box =  Gtk::Box (Gtk::Orientation::HORIZONTAL, 10);
            
            box.set_orientation (Gtk::Orientation::HORIZONTAL);
            box.append (label);
            box.append (scale);
            box.append (spin);

            GtkAdjustment * adj =  gtk_adjustment_new (control->val, control->min, control->max, .001, .001, 0);
            printf ("[controls] %f %f %f\n", control->val, control->min, control->max);
            
            spin.set_digits (2);
            
            gtk_spin_button_set_adjustment ((GtkSpinButton *)spin.gobj (), adj);
            gtk_range_set_adjustment ((GtkRange *)scale.gobj (), adj);
            // spin.set_adjustment (adj);
            box.set_spacing (0);

            card.append (box);
        }

        card.append (del);
    }

    void remove () ;
} ;

#endif
