#ifndef PRESETS_H
#define PRESETS_H
#include <gtkmm.h>
#include "logging_macros.h"

class Presets {
public:
    Gtk::Box master, my_presets, quick, library ;
    Gtk::Notebook notebook, presets ;
    Gtk::Button add ;
    void my () ;
    
    std::string dir ;
    
    Presets () {
        dir = std::string (getenv ("HOME")).append ("/.config/amprack") ;
        LOGD ("[presets] dir: %s\n", dir.c_str ());
        
        if (! g_file_test (dir.c_str (), G_FILE_TEST_IS_DIR)) {
            LOGD ("[presets] creating %s\n", dir.c_str ());
            if (g_mkdir_with_parents (dir.c_str (), 0777)) {
                LOGD ("error creating dir: %s\n", strerror (errno)) ;
            }
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
