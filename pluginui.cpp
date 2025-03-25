#include "pluginui.h"
#include "rack.h"

void select_model (GtkDropDown * down, int event, PluginUI * ui) {
    IN
    if (! GTK_IS_STRING_OBJECT (gtk_drop_down_get_selected_item ((GtkDropDown *)down))) {
        HERE wtf ("AAAAAAA\n");
        return ;
    }
    
    const char * filename = gtk_string_object_get_string ((GtkStringObject *)gtk_drop_down_get_selected_item ((GtkDropDown *)down));
    # ifdef __linux__
    std::string dir = std::string (getenv ("HOME")).append ("/amprack/models/").append (std::string (gtk_label_get_text (ui -> name))).append ("/").append (std::string (filename));
    # else
    std::string dir = std::string (getenv ("USERPROFILE")).append ("/amprack/models/").append (std::string (gtk_label_get_text (ui -> name))).append ("/").append (std::string (filename));
    # endif
    
    wtf ("[model] %s\n", dir.c_str ());
    
    if (ui -> engine -> activePlugins -> at (ui -> index) -> loadedFileType)
        ui -> engine -> set_plugin_file (ui-> index, (char *)dir.c_str ());
    else
        ui -> engine -> set_plugin_audio_file (ui-> index, (char *)dir.c_str ());

    OUT
}

void dropdown_next (GtkDropDown * dropdown) {
    gtk_drop_down_set_selected (dropdown, gtk_drop_down_get_selected (dropdown) + 1) ;
}

void dropdown_prev (GtkDropDown * dropdown) {
    gtk_drop_down_set_selected (dropdown, gtk_drop_down_get_selected (dropdown) - 1) ;
}

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

  gtk_box_remove (cd -> parent, (GtkWidget *)cd -> card);
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
    wtf ("[%s] %d : %d-> %f\n", gtk_label_get_text (ui -> name), ui -> get_index (), cd -> control, val);
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
      const char * _name = gtk_native_dialog_get_title (native);
      LOGV (filename);
      if (_name != nullptr)
        * ui->pType = PluginFileType::FILE_ATOM ;
        
      LOGD ("requested file type : %d\n", * ui -> pType) ;
      if ( *ui -> pType == 0) {
          LOGD ("[response] %d -> %s\n", * ui -> index_p, filename);
          ui -> engine ->set_plugin_audio_file (* ui -> index_p, filename);
      } else if (*ui -> pType == 1){
          ui -> engine ->set_plugin_file (ui -> index, filename);          
      } else {
          // this is fucking brilliant          
          int control =  _name[0];
          std::print ("[load atom] {}: {}", control, filename);
          ui -> engine -> set_atom_port (ui -> index, control, filename);
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
  const char * name = gtk_widget_get_name ((GtkWidget *) b) ;
  if (name != nullptr) {
      std::print ("[file chooser] {}\n", name);
      gtk_native_dialog_set_title ((GtkNativeDialog *)native, name) ;
  } else {
      std::print ("horse with no name\n");
  }

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
    return gtk_widget_get_name ((GtkWidget *) card) [0] - 48 ;
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

PluginUI::PluginUI (Engine * _engine, Plugin * _plugin, GtkBox * _parent, std::string pluginName, int _index, bool has_file_chooser, void * _rack) {    
    IN
    # ifdef __linux__
    std::string dir = std::string (getenv ("HOME")).append ("/amprack/models/").append (pluginName).append ("/");
    # else
    std::string dir = std::string (getenv ("USERPROFILE")).append ("/amprack/models/").append (pluginName).append ("/");
    # endif

    LOGD ("[plugin ui] loading %s\n", pluginName.c_str ());
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
    name = (GtkLabel *)gtk_label_new (pluginName.c_str ());
    GtkBox * nb = (GtkBox *)gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_widget_set_hexpand ((GtkWidget *)nb, true);
    gtk_label_set_wrap (name, true);
    gtk_box_append (nb, (GtkWidget *)name);
    //~ nb.append (name);
    gtk_widget_set_halign ((GtkWidget *) nb, GTK_ALIGN_START);
    
    auto n = std::string ("<big><b>").append (pluginName).append ("</b></big>");
    gtk_label_set_markup (name, n.c_str ());
    free (s);
    card =  (GtkBox *)gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
    card_ = (GtkWidget *)card ;
    
    //~ card.set_orientation (Gtk::Orientation::VERTICAL);
    //~ gtk_widget_add_css_class ((GtkWidget *) card.gobj (), "xwindow");

    GtkBox * container = (GtkBox *)gtk_box_new (GTK_ORIENTATION_VERTICAL, 10);
    
    gtk_box_append (card, (GtkWidget *)container);
    gtk_widget_set_name ((GtkWidget *) container, "plugin");
    
    //~ set_random_background ((GtkWidget *)container.gobj ());

    GtkBox * header = (GtkBox *)gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 10) ;
    gtk_box_append (container, (GtkWidget *)header);
    gtk_widget_set_hexpand ((GtkWidget *) header, true);
    //~ name.set_hexpand (true);
    gtk_label_set_justify (name, GTK_JUSTIFY_LEFT);
    
    gtk_box_append (header, (GtkWidget *) nb);
    gtk_widget_set_margin_bottom ((GtkWidget *) header, 10) ;
    gtk_widget_set_margin_top ((GtkWidget *) header, 10) ;
    gtk_widget_set_margin_end ((GtkWidget *) header, 10) ;
    gtk_widget_set_margin_start ((GtkWidget *) header, 0) ;

    onoff = (GtkSwitch *) gtk_switch_new ();
    gtk_switch_set_active (onoff, true);
    GtkBox * o_o = (GtkBox * )gtk_box_new (GTK_ORIENTATION_VERTICAL, 10);
    gtk_box_append (o_o, (GtkWidget *) onoff);
    gtk_box_append (header, (GtkWidget *) o_o) ;
    gtk_widget_set_halign ((GtkWidget *) onoff, GTK_ALIGN_END);
    gtk_widget_set_halign ((GtkWidget *)name, GTK_ALIGN_START);

    gtk_widget_set_margin_start ((GtkWidget *)card, 20);
    gtk_widget_set_margin_end ((GtkWidget *)card, 20);
    gtk_widget_set_margin_top ((GtkWidget *)card, 20);
    gtk_widget_set_margin_bottom ((GtkWidget *)card, 0);

    up = (GtkButton *) gtk_button_new_with_label ("↑");
    down = (GtkButton *) gtk_button_new_with_label ("↓");
    
    
    del = (GtkButton *) gtk_button_new_with_label ("Delete") ;
    gtk_widget_set_name ((GtkWidget *) del, "delete");

    gtk_widget_set_halign ((GtkWidget *) del, GTK_ALIGN_END);
    gtk_widget_set_valign ((GtkWidget *) del, GTK_ALIGN_END);
    gtk_widget_set_margin_top ((GtkWidget *) del, 10);
    gtk_widget_set_margin_start ((GtkWidget *) del, 10);
    gtk_widget_set_margin_bottom ((GtkWidget *) del, 10);
    gtk_widget_set_margin_end ((GtkWidget *) del, 10);

    gtk_widget_set_halign ((GtkWidget *)up, GTK_ALIGN_START);
    gtk_widget_set_valign ((GtkWidget *)up, GTK_ALIGN_START);
    gtk_widget_set_margin_top ((GtkWidget *) up, 5);
    gtk_widget_set_margin_start ((GtkWidget *) up, 5);
    gtk_widget_set_margin_bottom ((GtkWidget *) up, 5);
    gtk_widget_set_margin_end ((GtkWidget *) up, 5);
    gtk_widget_set_halign ((GtkWidget *)down, GTK_ALIGN_START);
    gtk_widget_set_valign ((GtkWidget *)down, GTK_ALIGN_START);
    gtk_widget_set_margin_top ((GtkWidget *) down, 5);
    gtk_widget_set_margin_start ((GtkWidget *) down, 5);
    gtk_widget_set_margin_bottom ((GtkWidget *) down, 5);
    gtk_widget_set_margin_end ((GtkWidget *) down, 5);

    GtkButton * load_file = (GtkButton * )gtk_button_new_with_label ("Load file") ;

    gtk_widget_set_halign ((GtkWidget *) load_file, GTK_ALIGN_CENTER);
    gtk_widget_set_valign ((GtkWidget *) load_file, GTK_ALIGN_CENTER);
    gtk_widget_set_margin_bottom ((GtkWidget *) load_file, 20);
    
    g_signal_connect (load_file, "clicked", (GCallback) ui_file_chooser, this);

    // del.signal_clicked().connect(sigc::ptr_fun(&callback));
    CallbackData * cd = (CallbackData *) malloc (sizeof (CallbackData));
    cd->index = index ;
    cd -> card = card ;
    cd -> parent = parent ;
    cd->engine = engine;
    cd -> ui = (void*)this ;

    char name [3];
    name [0] = index + 48 ;
    name [1] = 0 ;
    
    gtk_widget_set_name ((GtkWidget * )card, name);

    g_signal_connect (del, "clicked", (GCallback) callback, cd);
    g_signal_connect (up, "clicked", (GCallback) pu_move_up, this);
    g_signal_connect (down, "clicked", (GCallback) pu_move_down, this);

    GtkBox * currentBox = NULL ;
    int row = 0, col = 0 ;
    
    for (int i = 0 ; i < plugin->pluginControls.size () ; i ++) {
        bool isAtom = false ;
        PluginControl * control = plugin->pluginControls.at (i) ;
        std::print ("[plugin control] {} type {}\n", i, plugin->pluginControls.at (i)->type);
        
        if (plugin->pluginControls.at (i)->type == PluginControl::Type::ATOM) {
            isAtom = true; 

            GtkLabel * label =  (GtkLabel * ) gtk_label_new ("");
            gtk_label_set_wrap (label, true);
            gtk_label_set_justify (label, GTK_JUSTIFY_CENTER);
            //~ LOGD ("control name: %s\n", control->lv2_name.c_str());
            gtk_label_set_label (label, control->lv2_name.c_str ());

            GtkButton * load_file = (GtkButton * )gtk_button_new_with_label ("Load file") ;

            gtk_widget_set_halign ((GtkWidget *) load_file, GTK_ALIGN_CENTER);
            gtk_widget_set_valign ((GtkWidget *) load_file, GTK_ALIGN_CENTER);
            gtk_widget_set_margin_bottom ((GtkWidget *) load_file, 20);

            char ** options = list_directory (dir);
            LOGD ("looking for models in %s\n", dir.c_str());
            //~ options [0] = strdup ("one");
            //~ options [1] = strdup ("two");
            //~ options [2] = nullptr ;
            
            GtkWidget * dropdown = gtk_drop_down_new_from_strings (options);
            //~ char * f = options [0] ;            
            int ii = 0 ;
            while (options [++ii]!= nullptr)
                free (options [ii]);
            //~ while (f != nullptr) {
                //~ free (f);
                //~ f++ ;
            //~ }
            
            char * name = (char *) malloc (3) ;
            name [0] = i ;
            name [1] = 0 ;
            
            gtk_widget_set_name ((GtkWidget *)load_file, name);
            g_signal_connect (load_file, "clicked", (GCallback) ui_file_chooser, this);

            GtkBox * box = (GtkBox * ) gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 10);
            gtk_box_append (container, (GtkWidget *)label);
            gtk_box_append (container, (GtkWidget *)box);
            gtk_box_append (container, (GtkWidget *)load_file);
            
            gtk_widget_set_hexpand ((GtkWidget *) box, true);
            gtk_widget_set_hexpand ((GtkWidget *) dropdown, true);
            
            GtkWidget * prev = (GtkWidget *) gtk_button_new ();
            GtkWidget * next = (GtkWidget *) gtk_button_new ();
            
            gtk_button_set_label ((GtkButton *) prev, "<");
            gtk_button_set_label ((GtkButton *) next, ">");
            
            gtk_widget_set_margin_end (prev, 10);
            gtk_widget_set_margin_start (prev, 10);
            gtk_widget_set_margin_top (prev, 20);
            gtk_widget_set_margin_top (next, 20);
            gtk_widget_set_margin_start (next, 10);
            
            g_signal_connect_swapped (prev, "clicked", (GCallback) dropdown_prev, dropdown);
            g_signal_connect_swapped (next, "clicked", (GCallback) dropdown_next, dropdown);
            
            gtk_box_append (box, (GtkWidget *) prev);
            gtk_box_append (box, (GtkWidget *) dropdown);
            gtk_box_append (box, (GtkWidget *) next);
            
            pType = (PluginFileType *) malloc (sizeof (PluginFileType));
            *pType = FILE_ATOM ;
            
            continue ;
                        
        }

        // Gtk::Adjustment  adj =  Gtk::Adjustment (control->val, control->min, control->max, 1, 1, 1);
        GtkAdjustment * adj =  gtk_adjustment_new (control->val, control->min, control->max, .001, .001, 0);
        GtkScale * scale =  (GtkScale * )gtk_scale_new (GTK_ORIENTATION_HORIZONTAL, adj);
        GtkSpinButton * spin = (GtkSpinButton * ) gtk_spin_button_new (adj, .001, 1);
        sliders.push_back (scale);
        gtk_range_set_value ((GtkRange *)scale, control->val);
        gtk_widget_set_hexpand ((GtkWidget *)scale, true);

        GtkLabel * label =  (GtkLabel * ) gtk_label_new ("");
        gtk_label_set_wrap (label, true);
        gtk_label_set_justify (label, GTK_JUSTIFY_CENTER);
        //~ LOGD ("control name: %s\n", control->lv2_name.c_str());
        if (plugin->type == SharedLibrary::PluginType::LILV) {
            gtk_label_set_label (label, control->lv2_name.c_str ());
        } else {
            gtk_label_set_label (label, control->name);
        }
        
        GtkBox * box = (GtkBox * ) gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 10);
        //~ if (niceRack->bnobs) {
            //~ box.set_orientation (Gtk::Orientation::VERTICAL);
        //~ }
                    
        GtkWidget * dropdown = nullptr ;
        //~ LOGD ("searching for %s in amps\n", pluginName.c_str ());

        if (engine -> amps.contains (pluginName.c_str ())) {
            wtf ("[mmmph] drop down\n");
            //~ LOGD ("plugin found, looking for control: %d\n", i);
            json mod = engine -> amps [pluginName] ;
            if (mod.contains (std::to_string (i))) {
                auto c = mod [std::to_string (i)];
                //~ std::cout << c << std::endl;
                char * options [1000] ;
                
                int x = 0 ;
                for (auto val: c) {
                    wtf ("[drop down] %d:%s\n", x,  val.dump().c_str ());
                    //~ std::cout << val << std::endl;
                    options [x] = strdup ((char *)val.dump ().c_str ()) ;
                    x ++ ;
                    if (x > 999)
                        break ;
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
            #ifdef __linux__
            if (strcasestr (control -> name, bypassContains [y]) != NULL) {
                bypassBtn = (GtkToggleButton *) gtk_toggle_button_new_with_label (control->name);
            }
            #else
            if (stricmp (control -> name, bypassContains [y]) == 0) {
                bypassBtn = (GtkToggleButton *) gtk_toggle_button_new_with_label (control->name);
            }
            
            #endif
            
            y ++ ;
        }
        
        KnobWidget * knob = nullptr;
        int layout_size = 1 ;
        if (plugin->pluginControls.size () < 15) {
            auto kInfo = engine -> knobs [std::to_string (plugin->pluginControls.size ())];
            auto layout = kInfo [std::to_string (row)];
            layout_size = layout.size ();
            
            //~ wtf ("[layout] %d x %d\n", col, row);
            
            int kSize = knob_sizes [layout [col].get <int>()];
            knob = knob_widget_new_simple (
                knob_get, knob_get, knob_set, spin, control->min, control->max, kSize, 100);

            gtk_widget_show ((GtkWidget *)knob);
            gtk_widget_set_name ((GtkWidget *)knob, std::string ("knob").append (std::to_string (layout [col].get <int>())).c_str ());
            knob->arc = false;
        } else {
            //~ wtf ("[layout] too many controls!\n");
        }
        
        if (! niceRack -> bnobs || dropdown != nullptr || bypassBtn != nullptr || knob == nullptr) {
            gtk_box_append (box, (GtkWidget *) label);
            gtk_box_append (box, (GtkWidget *) scale);
            gtk_widget_set_hexpand ((GtkWidget *) spin, false);
            gtk_widget_set_halign ((GtkWidget *) spin, GTK_ALIGN_CENTER);
            GtkBox * tmp = (GtkBox * )gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
            gtk_widget_set_hexpand ((GtkWidget *) tmp, false);
            gtk_widget_set_vexpand ((GtkWidget *) tmp, false);
            gtk_widget_set_halign ((GtkWidget *) tmp, GTK_ALIGN_CENTER);
            
            gtk_box_append (tmp, (GtkWidget *)spin);
            gtk_box_append (box, (GtkWidget *) tmp);
            
            if (bypassBtn != nullptr) {
                gtk_widget_set_hexpand ((GtkWidget *) tmp, true);
                gtk_box_append (tmp, (GtkWidget *) bypassBtn);
                gtk_widget_set_visible ((GtkWidget *) spin, false);
                gtk_widget_set_visible ((GtkWidget *) scale, false);
                gtk_widget_set_visible ((GtkWidget *) label, false);
                gtk_widget_set_name ((GtkWidget *)bypassBtn, "bypass");
                g_signal_connect (bypassBtn, "toggled", (GCallback )bypass_cb, spin);
                gtk_widget_set_halign ((GtkWidget *) bypassBtn, GTK_ALIGN_CENTER);
            }
        } else if (knob != nullptr) {
            if (currentBox == nullptr) {
                currentBox = (GtkBox *)gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 10);
                gtk_box_set_homogeneous (currentBox, false);
                gtk_widget_set_halign ((GtkWidget *)currentBox, GTK_ALIGN_FILL);
                gtk_box_append (box, (GtkWidget *)currentBox);
                gtk_widget_set_margin_top ((GtkWidget * )currentBox, 20);
            }
               
            GtkBox * rowBox = (GtkBox *)gtk_box_new (GTK_ORIENTATION_VERTICAL, 10);
            gtk_widget_set_halign ((GtkWidget *)rowBox, GTK_ALIGN_FILL);
            gtk_widget_set_hexpand ((GtkWidget *) rowBox, true);

            gtk_widget_set_hexpand ((GtkWidget *) spin, false);
            gtk_widget_set_halign ((GtkWidget *) spin, GTK_ALIGN_CENTER);
            
            gtk_box_append (currentBox, (GtkWidget *)rowBox);
            gtk_box_append (rowBox, (GtkWidget *)scale);
            gtk_box_append (rowBox, (GtkWidget *)spin);      
            gtk_widget_set_hexpand ((GtkWidget *)spin, false);
            gtk_box_append (rowBox, (GtkWidget *)knob);
            gtk_box_append (rowBox, (GtkWidget *)label);
            
            //~ spin.hide ();
            gtk_widget_set_visible ((GtkWidget *)scale, false);
            gtk_label_set_width_chars ((GtkLabel *) spin, 3);
        }
        
        //~ wtf ("[layout] %d: %d\n", col, layout.size ());
        col ++ ;
        if (col >= layout_size) {
            currentBox = nullptr;
            col = 0 ;
            row ++ ;
        }
        
        //~ printf ("[controls] %f %f %f\n", control->val, control->min, control->max);
        
        if (dropdown != nullptr) {
            GtkWidget * prev = (GtkWidget *) gtk_button_new ();
            GtkWidget * next = (GtkWidget *) gtk_button_new ();
            
            gtk_button_set_label ((GtkButton *) prev, "<");
            gtk_button_set_label ((GtkButton *) next, ">");
            
            gtk_widget_set_margin_end (prev, 10);
            gtk_widget_set_margin_start (prev, 10);
            gtk_widget_set_margin_top (prev, 20);
            gtk_widget_set_margin_top (next, 20);
            gtk_widget_set_margin_start (next, 10);
            
            g_signal_connect_swapped (prev, "clicked", (GCallback) dropdown_prev, dropdown);
            g_signal_connect_swapped (next, "clicked", (GCallback) dropdown_next, dropdown);
            
            gtk_box_append (box, (GtkWidget *) prev);
            gtk_box_append (box, (GtkWidget *) dropdown);
            gtk_box_append (box, (GtkWidget *) next);
            
            gtk_widget_set_visible ((GtkWidget *) scale, false);
            gtk_widget_set_visible ((GtkWidget *) spin, false);
            gtk_widget_set_hexpand ((GtkWidget *) dropdown, true);
            
            gtk_widget_set_margin_top ((GtkWidget * )label, 20);
            gtk_widget_set_margin_top ((GtkWidget * )dropdown, 20);
            g_signal_connect (dropdown, "notify::selected", (GCallback) dropdown_activated, adj);
            gtk_widget_set_margin_start (dropdown, 10);
        }
        
        CallbackData * cd = (CallbackData *) malloc (sizeof (CallbackData));
        cd->index = index ;
        cd -> card = card ;
        cd -> parent = parent ;
        cd->engine = engine;
        cd->control = i ;
        cd -> dropdown = dropdown ;
        cd -> ui = (void *) this ;
        
        gtk_spin_button_set_digits (spin, 2);
        
        gtk_spin_button_set_adjustment ((GtkSpinButton *)spin, adj);
        gtk_range_set_adjustment ((GtkRange *)scale, adj);
        // spin.set_adjustment (adj);
        
        g_signal_connect (scale, "value-changed", (GCallback) control_changed, cd) ;
        g_signal_connect (onoff, "state-set", (GCallback) bypass, cd) ;

        // wtf fr?
        if (not isAtom)
            gtk_box_append (container, (GtkWidget *)box);
    }

    GtkBox * bbox = (GtkBox * )gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_set_halign ((GtkWidget *) bbox, GTK_ALIGN_START);
    
    gtk_box_append (container, (GtkWidget *)del);
    gtk_box_append (container, (GtkWidget *)bbox);
    gtk_box_append (bbox, (GtkWidget *)up);
    gtk_box_append (bbox, (GtkWidget *)down);
    
    if (has_file_chooser) {
        wtf ("[file chooser] mmmmph\n");
        GtkWidget * box = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 10);
        gtk_widget_set_hexpand (box, true);
        
        char ** entries = list_directory (dir);
        GtkWidget * down = gtk_drop_down_new_from_strings (entries) ;
        gtk_widget_set_hexpand (down, true);
        g_signal_connect (down, "notify::selected", (GCallback) select_model, this);

        GtkWidget * prev = (GtkWidget *) gtk_button_new ();
        GtkWidget * next = (GtkWidget *) gtk_button_new ();
        
        gtk_button_set_label ((GtkButton *) prev, "<");
        gtk_button_set_label ((GtkButton *) next, ">");
        
        //~ gtk_widget_set_margin_end (prev, 10);
        //~ gtk_widget_set_margin_start (prev, 10);
        //~ gtk_widget_set_margin_top (prev, 20);
        //~ gtk_widget_set_margin_top (next, 20);
        //~ gtk_widget_set_margin_start (next, 10);
        
        g_signal_connect_swapped (prev, "clicked", (GCallback) dropdown_prev, down);
        g_signal_connect_swapped (next, "clicked", (GCallback) dropdown_next, down);
        
        GtkScrolledWindow * sw_down = (GtkScrolledWindow *) gtk_scrolled_window_new ();
        
        gtk_box_append (container, (GtkWidget *)sw_down);
        gtk_scrolled_window_set_child (sw_down, box);
        
        //~ gtk_widget_set_vexpand ((GtkWidget *) box, true);
    
        GtkWidget * dh_box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
        gtk_widget_set_margin_bottom (prev, 20);
        gtk_widget_set_margin_bottom (next, 20);
        gtk_box_append ((GtkBox *) dh_box, down);
        
        gtk_box_append ((GtkBox *)box, prev);
        gtk_box_append ((GtkBox *)box, dh_box);
        gtk_box_append ((GtkBox *)box, next);
        gtk_box_append ((GtkBox *)container, (GtkWidget *)load_file);
        
        int i = 0 ; 
        while (entries [i] != NULL) {
            free (entries [i++]);
        }
        
        free (entries);
    }
    
    OUT
}
