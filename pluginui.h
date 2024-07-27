#ifndef PLUGINUI_H
#define PLUGINUI_H

#include <gtkmm.h>
#include <gtk/gtk.h>
#include <vector>
#include <iostream>
#include <sstream>

#include "json.hpp"
#include "log.h"
#include "defs.h"
#include "engine.h"
#include "Plugin.h"
#include "PluginControl.h"
#include "callback_data.h"

using json = nlohmann::json;

void callback (void * p, void *c);
void bypass (void * p, bool, void * c)  ;
void control_changed (void * p, void * c);
void ui_file_chooser (void * b, void * d)  ;
void dropdown_activated (void * d, int, void * s)  ;

typedef enum {
    FILE_AUDIO,
    FILE_JSON
} PluginFileType ;

class PluginUI {
public:    
    Gtk::Label name ;
    Gtk::Switch  onoff ;
    Gtk::Button  del,  up,  down ;
    Engine * engine ;
    Plugin * plugin ;
    Gtk::Box  card ;
    Gtk::Box * parent ;
    int index, * index_p ;
    PluginFileType * pType ;
    std::vector <GtkScale *> sliders ;
  
    void load_preset (std::string);
  
    PluginUI (Engine * _engine, Plugin * _plugin, Gtk::Box * _parent, std::string pluginName, int _index, bool has_file_chooser) {
        engine = _engine ;
        plugin = _plugin ;
        index = _index ;
        index_p = (int *) malloc (sizeof (int));
        *index_p = index ;
        parent = _parent ;

        char * s = (char *) malloc (pluginName.size () + 3) ;
        sprintf (s, "%d %s", index, pluginName.c_str ());
        //~ printf ("[plugin ui] %d %s", index, pluginName.c_str ());
        // name =  Gtk::Label (s) ;
        name = Gtk::Label (pluginName);
        Gtk::Box nb = Gtk::Box (Gtk::Orientation::HORIZONTAL, 10);
        nb.set_hexpand (true);
        nb.append (name);
        nb.set_halign (Gtk::Align::START);
        
        auto n = std::string ("<big><b>").append (pluginName).append ("</b></big>");
        name.set_markup (n.c_str ());
        free (s);
        card =  Gtk::Box (Gtk::Orientation::VERTICAL, 0);
        card.set_orientation (Gtk::Orientation::VERTICAL);
        Gtk::Box header = Gtk::Box (Gtk::Orientation::HORIZONTAL, 10) ;
        card.append (header);
        header.set_hexpand (true);
        //~ name.set_hexpand (true);
        name.set_justify (Gtk::Justification::LEFT);
        header.append (nb);
        header.set_margin (10);
        header.set_margin_start (0);

        onoff = Gtk::Switch () ;
        onoff.set_active (true);
        header.append (onoff) ;
        onoff.set_halign (Gtk::Align::END);
        name.set_halign (Gtk::Align::START);

        card.set_margin (10);
        card.set_margin_bottom (0);
        
        del = Gtk::Button ("Delete") ;

        del.set_halign (Gtk::Align::END);
        del.set_valign (Gtk::Align::END);
        del.set_margin (10);

        Gtk::Button load_file = Gtk::Button ("Load file") ;

        load_file.set_halign (Gtk::Align::CENTER);
        load_file.set_valign (Gtk::Align::CENTER);
        load_file.set_margin (10);
        
        g_signal_connect (load_file.gobj (), "clicked", (GCallback) ui_file_chooser, this);

        // del.signal_clicked().connect(sigc::ptr_fun(&callback));
        CallbackData * cd = (CallbackData *) malloc (sizeof (CallbackData));
        cd->index = index ;
        cd -> card = & card ;
        cd -> parent = parent ;
        cd->engine = engine;

        card.set_name ("hello");

        g_signal_connect (del.gobj (), "clicked", (GCallback) callback, cd);

        for (int i = 0 ; i < plugin->pluginControls.size () ; i ++) {
            PluginControl * control = plugin->pluginControls.at (i) ;
            Gtk::Scale  scale =  Gtk::Scale () ;
            sliders.push_back (scale.gobj ());
            scale . set_value (control->val);
            scale.set_hexpand (true);

            // Gtk::Adjustment  adj =  Gtk::Adjustment (control->val, control->min, control->max, 1, 1, 1);
            Gtk::SpinButton  spin =  Gtk::SpinButton ();

            Gtk::Label  label =  Gtk::Label ();
            //~ LOGD ("control name: %s\n", control->lv2_name.c_str());
            if (plugin->type == SharedLibrary::PluginType::LILV) {
                label.set_label (control->lv2_name.c_str ());
            } else {
                label.set_label (control->name);
            }
            
            Gtk::Box  box =  Gtk::Box (Gtk::Orientation::HORIZONTAL, 10);
                        
            GtkWidget * dropdown = nullptr ;
            //~ LOGD ("searching for %s in amps\n", pluginName.c_str ());

            if (engine -> amps.contains ("rkr Cabinet")) {
                //~ LOGD ("plugin found, looking for control: %d\n", i);
                json mod = engine -> amps [pluginName] ;
                if (mod.contains (std::to_string (i))) {
                    auto c = mod [std::to_string (i)];
                    //~ std::cout << c << std::endl;
                    char * options [1000] ;
                    
                    int x = 0 ;
                    for (auto val: c) {
                        //~ std::cout << val << std::endl;
                        options [x] = strdup ((char *)val.dump ().c_str ()) ;
                        x ++ ;
                    }
                    
                    options [x] = NULL ;
                    dropdown = gtk_drop_down_new_from_strings (options) ;
                    
                    while (--x != 0)
                        free (options [x]);
                }
            }
            
            box.set_orientation (Gtk::Orientation::HORIZONTAL);
            box.append (label);
            
            box.append (scale);
            box.append (spin);

            GtkAdjustment * adj =  gtk_adjustment_new (control->val, control->min, control->max, .001, .001, 0);
            //~ printf ("[controls] %f %f %f\n", control->val, control->min, control->max);
            
            if (dropdown != nullptr) {
                gtk_box_append (box.gobj (), dropdown);
                scale.set_visible (false);
                spin.set_visible (false);            
                
                g_signal_connect (dropdown, "notify::selected", (GCallback) dropdown_activated, adj);
                gtk_widget_set_margin_start (dropdown, 10);
            }
            
            CallbackData * cd = (CallbackData *) malloc (sizeof (CallbackData));
            cd->index = index ;
            cd -> card = & card ;
            cd -> parent = parent ;
            cd->engine = engine;
            cd->control = i ;
            cd -> dropdown = dropdown ;
            
            spin.set_digits (2);
            
            gtk_spin_button_set_adjustment ((GtkSpinButton *)spin.gobj (), adj);
            gtk_range_set_adjustment ((GtkRange *)scale.gobj (), adj);
            // spin.set_adjustment (adj);
            box.set_spacing (0);
            
            g_signal_connect (scale.gobj (), "value-changed", (GCallback) control_changed, cd) ;
            g_signal_connect (onoff.gobj (), "state-set", (GCallback) bypass, cd) ;

            card.append (box);
        }

        card.append (del);
        if (has_file_chooser)
            card.append (load_file);
    }

    void remove () ;
} ;

#endif
