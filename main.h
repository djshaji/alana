#ifndef MAIN_H
#define MAIN_H

#include <gtkmm.h>
#include "log.h"
#include "rack.h"
#include "presets.h"


class MyWindow : public Gtk::Window
{
public:
    Gtk::Paned  pane ; 
    Gtk::Stack  stack ;
    Gtk::StackSwitcher  switcher ;
    Gtk::Box box, stack_box ;
    Rack * rack ;
    Presets presets ; 
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
    
    presets = Presets () ;
    gtk_notebook_append_page (presets.notebook.gobj (), rack->rack, gtk_label_new ("Effects"));
    
    presets.my () ;
    
    pane.set_start_child (presets.master);
    pane.set_end_child (rack->master);
}

#endif
