#ifndef MAIN_H
#define MAIN_H

#include <gtkmm.h>
#include "log.h"
#include "rack.h"
#include "presets.h"

class CB {
    public:
        Presets * presets ;
        Engine * engine ;
};

void save_preset_cb (void * w, void * d) {
    CB * cb = (CB *) d ;
    cb -> engine -> savePreset (std::string (cb -> presets -> presets_dir).append ("test"), "description");
}

class MyWindow : public Gtk::Window
{
public:
    Gtk::Paned  pane ; 
    Gtk::Stack  stack ;
    Gtk::StackSwitcher  switcher ;
    Gtk::Box box, stack_box ;
    Rack * rack ;
    Presets * presets ; 
  MyWindow();
};

MyWindow::MyWindow()
{
    set_title("Amp Rack 5 (Alpha)");
    set_default_size(1200, 800);

    box = Gtk::Box () ;
    set_child (box);

    pane = Gtk::Paned () ;
    box.set_orientation (Gtk::Orientation::VERTICAL);
    pane.set_orientation (Gtk::Orientation::HORIZONTAL);

    stack_box = Gtk::Box () ;
    stack_box.set_spacing (10);
    box.append (stack_box);
    stack_box.set_orientation (Gtk::Orientation::VERTICAL);
    
    pane.set_position (600);

    stack = Gtk::Stack () ;
    stack_box.append (stack);
    stack_box.set_homogeneous (false);

    switcher = Gtk::StackSwitcher () ;
    stack_box.append (switcher);

    rack = new Rack () ;
    stack.add (pane);
    
    set_titlebar (rack->button_box);
    
    presets = new Presets () ;
    presets->engine = rack -> engine ;
    presets->rack = rack ;
    
    gtk_notebook_append_page (presets->notebook.gobj (), rack->rack, gtk_label_new ("Effects"));
    
    presets->my () ;
    
    CB * cb = new CB () ;
    cb -> engine = rack -> engine ;
    cb -> presets = presets ;
    
    g_signal_connect (presets->add.gobj (), "clicked", (GCallback) save_preset_cb, cb);
    
    pane.set_start_child (presets->master);
    pane.set_end_child (rack->master);
}

#endif
