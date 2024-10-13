#ifndef MAIN_H
#define MAIN_H
#include <filesystem>

//~ #include <gtkmm.h>
#include <gtk/gtk.h>
#include "log.h"
#include "rack.h"
#include "presets.h"
#include "settings.h"

using json = nlohmann::json;

class CB {
    public:
        Presets * presets ;
        Engine * engine ;
};


void add_cb (GtkDialog* self, gint response_id, gpointer user_data) {
    Alert_CB * cb = (Alert_CB *) user_data ;
    char * filename = (char *)gtk_entry_buffer_get_text ((GtkEntryBuffer *) gtk_entry_get_buffer ((GtkEntry *) cb -> widget));
    char * desc = (char *)gtk_entry_buffer_get_text ((GtkEntryBuffer *) gtk_entry_get_buffer ((GtkEntry *) cb -> widget2));
    if (response_id == -6 /* ?? */ || strlen (filename) == 0) {
        gtk_window_destroy ((GtkWindow *)cb->dialog);
        delete (cb);
        return ;
    }
    
    LOGD ("[cb] id: %d\n", response_id);
    Presets * presets = (Presets *) cb -> data;
    std::string f = std::string (presets -> presets_dir->c_str ()).append (filename) ;
    presets -> engine -> savePreset (f, std::string (desc));    
    json j = filename_to_json (f);
    presets->add_preset (j, 1);

    gtk_window_destroy ((GtkWindow *)cb->dialog);
    delete (cb);
}


void preset_name (gpointer callback, gpointer data) {
    GtkDialogFlags flags = (GtkDialogFlags) (GTK_DIALOG_DESTROY_WITH_PARENT | GTK_DIALOG_MODAL);
    GtkWidget * dialog = gtk_message_dialog_new (null,
                                     (GtkDialogFlags)flags,
                                     GTK_MESSAGE_INFO,
                                     GTK_BUTTONS_OK_CANCEL,
                                     "Add Preset",
                                     null);
    // Destroy the dialog when the user responds to it
    // (e.g. clicks a button)

    Alert_CB * cb = new Alert_CB () ;
    cb -> data = data ;

    GtkWidget * box = (GtkWidget *)gtk_box_new (GTK_ORIENTATION_VERTICAL, 10) ;
    gtk_box_append ((GtkBox *)gtk_dialog_get_content_area ((GtkDialog *)dialog), box);
    
    GtkWidget * label = gtk_label_new ("Enter preset name");
    gtk_box_append ((GtkBox *)gtk_message_dialog_get_message_area ((GtkMessageDialog *)dialog), label);
    
    GtkWidget * entry = gtk_entry_new ();
    gtk_box_append ((GtkBox *)box, entry);    

    GtkWidget * desc = gtk_entry_new ();
    gtk_box_append ((GtkBox *)box, gtk_label_new ("Enter Description"));    
    gtk_box_append ((GtkBox *)box, desc);    

    gtk_widget_set_margin_bottom ((GtkWidget *) box, 10);
    gtk_widget_set_margin_start ((GtkWidget *) box, 10);
    gtk_widget_set_margin_end ((GtkWidget *) box, 10);
    gtk_widget_set_margin_top ((GtkWidget *) box, 10);

    cb -> widget = entry ;
    cb -> widget2 = desc ;
    cb -> dialog = dialog ;
    g_signal_connect (dialog, "response",
                      G_CALLBACK (callback),
                      cb);
    
    gtk_widget_set_size_request (dialog, 400, 350);
    gtk_window_present ((GtkWindow *) dialog);
}

void save_preset_cb (void * w, void * d) {
    CB * cb = (CB *) d ;
    preset_name ((void *)add_cb, cb ->presets);
    //~ cb -> engine -> savePreset (std::string (cb -> presets -> presets_dir).append ("test"), "description");
}

class MyWindow
{
public:
    GtkWindow * window ;
    GtkPaned * pane ; 
    GtkStack * stack ;
    GtkStackSwitcher * switcher ;
    GtkBox * box, * stack_box ;
    GtkApplication * app ;
    Rack * rack ;
    Presets * presets ; 
    GtkWindow * gobj ();
  MyWindow (GtkApplication *);
};

void onshow (void * w, void * d) {
    IN
    MyWindow * window = (MyWindow *) d ;    
    std::string default_preset = std::string (window ->presets -> dir) .append ("/default") ;
    window -> rack -> load_preset (default_preset);

    json favs = filename_to_json (std::string (window -> presets -> dir).append ("/fav.json"));
    for (int i = 0 ; i < window -> rack -> hearts.size (); i ++) {
        GtkWidget * fav = (GtkWidget *) window -> rack -> hearts.at (i);
        if (favs.contains (gtk_widget_get_name (fav))) {
            gtk_toggle_button_set_active ((GtkToggleButton *) fav, true);
        }
    }
    
    OUT
}

void qquit (void *) {
    //~ abort () ;
}

void quit (void * w, void * d) {
    MyWindow * window = (MyWindow *) d ;
    window -> rack -> engine -> driver -> deactivate ();
    window -> rack -> engine -> driver -> close ();
    window -> rack -> engine -> savePreset (std::string (window -> presets -> dir) .append ("/default").c_str (), "Last saved preset") ;
    
    json favs = {} ;
    
    for (int i = 0 ; i < window -> rack -> hearts.size (); i ++) {
        GtkWidget * fav = (GtkWidget *) window -> rack -> hearts.at (i);
        if (gtk_toggle_button_get_active ((GtkToggleButton *)fav)) {
            favs [gtk_widget_get_name (fav)] = true ;
        }
    }
    
    json_to_filename (favs, std::string (window -> presets -> dir).append ("/fav.json"));    
    # ifdef __linux__
    json_to_filename (window -> rack->config, std::string (getenv ("HOME")).append ("/.config/amprack/config.json"));    
    # else
    json_to_filename (window -> rack->config, std::string (getenv ("USERPROFILE")).append ("/.config/amprack/config.json"));    
    # endif
    
    gtk_window_destroy ((GtkWindow *)window -> gobj ());
    g_application_quit ((GApplication *)window -> app);
}

// omg
// types can be used directly like this
// i love C
static gboolean
hotkeys (MyWindow             *window,
                      guint                  keyval,
                      guint                  keycode,
                      GdkModifierType        state,
                      GtkEventControllerKey *event_controller)
{
    printf ("[keypress] %d\n", keyval);
    switch (keyval) {
        case 65365:
            window -> rack -> next_preset ();
            break ;
        case 65366:
            window -> rack -> prev_preset ();
            break ;
        case 65307:
            quit (NULL, window);
            break;
        case 49:
            gtk_notebook_set_current_page (window -> presets -> notebook, 1);
            gtk_notebook_set_current_page (window -> presets -> presets, 0);
            break ;
        case 50:
            gtk_notebook_set_current_page (window -> presets -> notebook, 1);
            gtk_notebook_set_current_page (window -> presets -> presets, 1);
            break ;
        case 51:
            gtk_notebook_set_current_page (window -> presets -> notebook, 1);
            gtk_notebook_set_current_page (window -> presets -> presets, 2);
            break ;
        case 52:
            gtk_notebook_set_current_page (window -> presets -> notebook, 1);
            gtk_notebook_set_current_page (window -> presets -> presets, 3);
            break ;
        case 96:
            gtk_notebook_set_current_page (window -> presets -> notebook, 0);
            gtk_notebook_set_current_page (window -> presets -> presets, 0);
            break ;
    }
    
    return true;
}

#endif

