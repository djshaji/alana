#ifndef PRESETS_H
#define PRESETS_H
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
    GtkBox * master, * my_presets, * quick, * library, * my_presets_rack, * favorites ;
    GtkNotebook * notebook, * presets ;
    GtkButton * add, * imp, * exp ;
    void my () ;
    GtkSpinner * library_spinner ;
    void * _this ;
    Engine * engine ;
    Rack * rack ;
    GtkApplication * app ;
    std::vector <GtkWidget *> library_boxes ;
    int page = 0;
    json library_json ;
    
    
    void add_preset (json, int);
    void load_user (bool);
    void add_preset_multi (json, int) ;
    void add_preset_multi (std::string, int) ;
    int import_presets_from_json (json);
    void save_presets_to_json (std::string);
    std::string dir, favs_dir ;
    static std::string * presets_dir ;
    std::vector <json> * list_of_presets [4] ;
    char * _pdir ;
    void library_load () ;
    const int page_size = 50 ;
    GtkWidget * page_no ;
    GtkAdjustment * adj ;
    
    json get_all_user_presets ();
    
    Presets () {
        # ifdef __linux__
        dir = std::string (getenv ("HOME")).append ("/.config/amprack") ;
        # else
        dir = std::string (getenv ("USERPROFILE")).append ("/.config/amprack") ;
        # endif
        
        LOGD ("[presets] dir: %s\n", dir.c_str ());
        
        # ifdef __linux__
        presets_dir = new std::string (getenv ("HOME"));
        presets_dir->append ("/amprack/presets/");
        # else
        presets_dir = new std::string (getenv ("USERPROFILE"));
        presets_dir->append ("\\amprack\\presets\\");
        # endif
        
        LOGD ("[presets dir] %s\n", presets_dir->c_str ());
        
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
        
        master = (GtkBox *)gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0) ;
        
        notebook = (GtkNotebook *)gtk_notebook_new ();
        gtk_box_append (master, (GtkWidget *)notebook);
        
        //~ add = Gtk::Button ();
        //~ add.set_label ("+ Preset");
        //~ master.append (add);
    }
};

#endif
