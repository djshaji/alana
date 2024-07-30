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

void delete_callback (void * b, void * d) ;

class CB_Preset {
    public:
        Engine * engine ;
        json j ;
        Rack * rack ;
        GtkWidget * fav, * fav_rack ;
        void * p ;
        std::string filename ;
};

class Presets {
public:
    Gtk::Box master, my_presets, quick, library, my_presets_rack, favorites ;
    Gtk::Notebook notebook, presets ;
    Gtk::Button add, imp, exp ;
    void my () ;
    GtkSpinner * library_spinner ;
    void * _this ;
    Engine * engine ;
    Rack * rack ;
    GtkApplication * app ;
    
    void add_preset (json, int);
    void load_user (bool);
    void add_preset_multi (json, int) ;
    void add_preset_multi (std::string, int) ;
    void import_presets_from_json (json);
    void save_presets_to_json (std::string);
    std::string dir, * presets_dir, favs_dir ;
    char * _pdir ;
    
    Presets () {
        dir = std::string (getenv ("HOME")).append ("/.config/amprack") ;
        LOGD ("[presets] dir: %s\n", dir.c_str ());
        
        presets_dir = new std::string (dir);
        presets_dir->append ("/presets/");
        _pdir = strdup (presets_dir->c_str ());
        
        if (! g_file_test (dir.c_str (), G_FILE_TEST_IS_DIR)) {
            LOGD ("[presets] creating %s\n", dir.c_str ());
            if (g_mkdir_with_parents (dir.c_str (), 0777)) {
                LOGD ("error creating dir: %s\n", strerror (errno)) ;
            }        
        }
        
        if (g_mkdir_with_parents (presets_dir->c_str (), 0777)) {
            LOGD ("error creating dir: %s\n", strerror (errno)) ;
        }
        
        favs_dir = std::string (dir).append ("/favs") ;
        g_mkdir_with_parents (favs_dir.c_str (), 0777);
        
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
