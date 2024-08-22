#include "pluginui.h"
#include "rack.h"

void bypass_cb (void *toggle, void * spin) {
    if (gtk_toggle_button_get_active ((GtkToggleButton *) toggle)) {
        gtk_spin_button_set_value ((GtkSpinButton *) spin, 1) ;
    } else {
        gtk_spin_button_set_value ((GtkSpinButton *) spin, 0) ;
    }
}

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

float
control_port_get_snapped_val (Port * self)
{
   return 0 ;
}

void
control_port_set_real_val (Port * self, float val)
{
  
}

void knob_set (GtkSpinButton * w, float val) {
    gtk_spin_button_set_value (w, val);
}

float knob_get (GtkSpinButton * s) {
    return gtk_spin_button_get_value (s);
}

PluginUI::PluginUI (Engine * _engine, Plugin * _plugin, Gtk::Box * _parent, std::string pluginName, int _index, bool has_file_chooser, void * _rack) {    
    Rack * niceRack = (Rack *) _rack ;
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
    card_ = (GtkWidget *)card.gobj () ;
    
    card.set_orientation (Gtk::Orientation::VERTICAL);
    //~ gtk_widget_add_css_class ((GtkWidget *) card.gobj (), "xwindow");

    Gtk::Box container = Gtk::Box (Gtk::Orientation::VERTICAL, 10);
    
    card.append (container);
    container.set_name ("plugin");

    Gtk::Box header = Gtk::Box (Gtk::Orientation::HORIZONTAL, 10) ;
    container.append (header);
    header.set_hexpand (true);
    //~ name.set_hexpand (true);
    name.set_justify (Gtk::Justification::LEFT);
    header.append (nb);
    header.set_margin (10);
    header.set_margin_start (0);

    onoff = Gtk::Switch () ;
    onoff.set_active (true);
    Gtk::Box o_o = Gtk::Box (Gtk::Orientation::VERTICAL, 10);
    o_o.append (onoff);
    header.append (o_o) ;
    onoff.set_halign (Gtk::Align::END);
    name.set_halign (Gtk::Align::START);

    card.set_margin (20);
    card.set_margin_bottom (0);

    Gtk::Button up = Gtk::Button ("↑");
    Gtk::Button down = Gtk::Button ("↓");
    
    
    del = Gtk::Button ("Delete") ;
    del.set_name ("delete");

    del.set_halign (Gtk::Align::END);
    del.set_valign (Gtk::Align::END);
    del.set_margin (10);

    up.set_halign (Gtk::Align::START);
    up.set_valign (Gtk::Align::START);
    up.set_margin (5);
    down.set_halign (Gtk::Align::START);
    down.set_valign (Gtk::Align::START);
    down.set_margin (5);

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
    cd -> ui = (void*)this ;

    char name [3];
    name [0] = index + 48 ;
    name [1] = 0 ;
    
    card.set_name (name);

    g_signal_connect (del.gobj (), "clicked", (GCallback) callback, cd);
    g_signal_connect (up.gobj (), "clicked", (GCallback) pu_move_up, this);
    g_signal_connect (down.gobj (), "clicked", (GCallback) pu_move_down, this);

    GtkBox * currentBox = NULL ;
    int row = 0, col = 0 ;
    
    for (int i = 0 ; i < plugin->pluginControls.size () ; i ++) {
        PluginControl * control = plugin->pluginControls.at (i) ;
        Gtk::Scale  scale =  Gtk::Scale () ;
        sliders.push_back (scale.gobj ());
        scale . set_value (control->val);
        scale.set_hexpand (true);

        // Gtk::Adjustment  adj =  Gtk::Adjustment (control->val, control->min, control->max, 1, 1, 1);
        Gtk::SpinButton  spin =  Gtk::SpinButton ();

        Gtk::Label  label =  Gtk::Label ();
        label.set_wrap (true);
        label.set_justify (Gtk::Justification::CENTER);
        //~ LOGD ("control name: %s\n", control->lv2_name.c_str());
        if (plugin->type == SharedLibrary::PluginType::LILV) {
            label.set_label (control->lv2_name.c_str ());
        } else {
            label.set_label (control->name);
        }
        
        Gtk::Box  box =  Gtk::Box (Gtk::Orientation::HORIZONTAL, 10);
        //~ if (niceRack->bnobs) {
            //~ box.set_orientation (Gtk::Orientation::VERTICAL);
        //~ }
                    
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
        
        GtkToggleButton * bypassBtn = nullptr;
        int y = 0 ;
        while (bypassContains [y] != nullptr) {
            //~ wtf ("%s > %s = %d\n", bypassContains [y], control -> name, strcasestr (bypassContains [y], control -> name));
            if (strcasestr (control -> name, bypassContains [y]) != NULL) {
                bypassBtn = (GtkToggleButton *) gtk_toggle_button_new_with_label (control->name);
            }
            
            y ++ ;
        }
        
        KnobWidget * knob = nullptr;
        int layout_size = 1 ;
        if (plugin->pluginControls.size () < 15) {
            auto kInfo = engine -> knobs [std::to_string (plugin->pluginControls.size ())];
            auto layout = kInfo [std::to_string (row)];
            layout_size = layout.size ();
            
            wtf ("[layout] %d x %d\n", col, row);
            
            int kSize = knob_sizes [layout [col].get <int>()];
            knob = knob_widget_new_simple (
                knob_get, knob_get, knob_set, spin.gobj (), control->min, control->max, kSize, 100);

            gtk_widget_show ((GtkWidget *)knob);
            gtk_widget_set_name ((GtkWidget *)knob, std::string ("knob").append (std::to_string (layout [col].get <int>())).c_str ());
            knob->arc = false;
        } else {
            wtf ("[layout] too many controls!\n");
        }
        
        if (! niceRack -> bnobs || dropdown != nullptr || bypassBtn != nullptr || knob == nullptr) {
            box.append (label);
            box.append (scale);
            spin.set_hexpand (false);
            spin.set_halign (Gtk::Align::CENTER);
            Gtk::Box tmp = Gtk::Box (Gtk::Orientation::HORIZONTAL, 0);
            tmp.set_hexpand (false);
            tmp.set_vexpand (false);
            tmp.set_halign (Gtk::Align::CENTER);
            tmp.append (spin);
            box.append (tmp);
            
            if (bypassBtn != nullptr) {
                tmp.set_hexpand (true);
                gtk_box_append (tmp.gobj (), (GtkWidget *) bypassBtn);
                spin.set_visible (false);
                scale.set_visible (false);
                label.set_visible (false);
                gtk_widget_set_name ((GtkWidget *)bypassBtn, "bypass");
                g_signal_connect (bypassBtn, "toggled", (GCallback )bypass_cb, spin.gobj ());
                gtk_widget_set_halign ((GtkWidget *) bypassBtn, GTK_ALIGN_CENTER);
            }
        } else if (knob != nullptr) {
            if (currentBox == nullptr) {
                currentBox = (GtkBox *)gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 10);
                gtk_box_set_homogeneous (currentBox, false);
                gtk_widget_set_halign ((GtkWidget *)currentBox, GTK_ALIGN_FILL);
                gtk_box_append (box.gobj (), (GtkWidget *)currentBox);
                gtk_widget_set_margin_top ((GtkWidget * )currentBox, 20);
            }
               
            GtkBox * rowBox = (GtkBox *)gtk_box_new (GTK_ORIENTATION_VERTICAL, 10);
            gtk_widget_set_halign ((GtkWidget *)rowBox, GTK_ALIGN_FILL);
            gtk_widget_set_hexpand ((GtkWidget *) rowBox, true);

            spin.set_hexpand (false);
            spin.set_halign (Gtk::Align::CENTER);
            
            gtk_box_append (currentBox, (GtkWidget *)rowBox);
            gtk_box_append (rowBox, (GtkWidget *)scale.gobj ());
            gtk_box_append (rowBox, (GtkWidget *)spin.gobj ());      
            spin.set_hexpand (false);
            gtk_box_append (rowBox, (GtkWidget *)knob);
            gtk_box_append (rowBox, (GtkWidget *)label.gobj ());
            
            scale.hide ();
            spin.set_width_chars (3);
        }
        
        //~ wtf ("[layout] %d: %d\n", col, layout.size ());
        col ++ ;
        if (col >= layout_size) {
            currentBox = nullptr;
            col = 0 ;
            row ++ ;
        }
        
        GtkAdjustment * adj =  gtk_adjustment_new (control->val, control->min, control->max, .001, .001, 0);
        //~ printf ("[controls] %f %f %f\n", control->val, control->min, control->max);
        
        if (dropdown != nullptr) {
            gtk_box_append (box.gobj (), dropdown);
            scale.set_visible (false);
            spin.set_visible (false);            
            gtk_widget_set_hexpand ((GtkWidget *) dropdown, true);
            
            gtk_widget_set_margin_top ((GtkWidget * )label.gobj (), 20);
            gtk_widget_set_margin_top ((GtkWidget * )dropdown, 20);
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
        cd -> ui = (void *) this ;
        
        spin.set_digits (2);
        
        gtk_spin_button_set_adjustment ((GtkSpinButton *)spin.gobj (), adj);
        gtk_range_set_adjustment ((GtkRange *)scale.gobj (), adj);
        // spin.set_adjustment (adj);
        box.set_spacing (0);
        
        g_signal_connect (scale.gobj (), "value-changed", (GCallback) control_changed, cd) ;
        g_signal_connect (onoff.gobj (), "state-set", (GCallback) bypass, cd) ;

        container.append (box);
    }

    Gtk::Box bbox = Gtk::Box (Gtk::Orientation::HORIZONTAL, 0);
    bbox.set_halign (Gtk::Align::START);
    
    container.append (del);
    container.append (bbox);
    bbox.append (up);
    bbox.append (down);
    if (has_file_chooser)
        container.append (load_file);
}
