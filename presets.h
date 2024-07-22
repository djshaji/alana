#ifndef PRESETS_H
#define PRESETS_H
#include <gtkmm.h>
#include "logging_macros.h"
#include <filesystem>
#include "json.hpp"
#include <iostream>
#include "util.h"
#include "engine.h"
#include "rack.h"

using json = nlohmann::json;

class CB_Preset {
    public:
        Engine * engine ;
        json j ;
        Rack * rack ;
};

class Presets {
public:
    Gtk::Box master, my_presets, quick, library, my_presets_rack ;
    Gtk::Notebook notebook, presets ;
    Gtk::Button add ;
    void my () ;
    Engine * engine ;
    Rack * rack ;
    
    void add_preset (json, int);
    void load_user ();
    
    std::string dir, presets_dir ;
    
    Presets () {
        dir = std::string (getenv ("HOME")).append ("/.config/amprack") ;
        LOGD ("[presets] dir: %s\n", dir.c_str ());
        
        presets_dir = std::string (dir);
        presets_dir.append ("/presets/");
        
        if (! g_file_test (dir.c_str (), G_FILE_TEST_IS_DIR)) {
            LOGD ("[presets] creating %s\n", dir.c_str ());
            if (g_mkdir_with_parents (dir.c_str (), 0777)) {
                LOGD ("error creating dir: %s\n", strerror (errno)) ;
            }        
        }
        
        if (g_mkdir_with_parents (presets_dir.c_str (), 0777)) {
            LOGD ("error creating dir: %s\n", strerror (errno)) ;
        }
        
        master = Gtk::Box () ;
        master.set_orientation (Gtk::Orientation::VERTICAL);
        
        notebook = Gtk::Notebook () ;
        master.append (notebook);
        
        //~ add = Gtk::Button ();
        //~ add.set_label ("+ Preset");
        //~ master.append (add);
    }
};

#endif
