#include "main.h"
#include "sync.h"

const char * renderers [7] = {
    "auto",
    "cairo",
    "gl",
    "ngl",
    "vulkan",
    nullptr
} ;


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
    IN
    MyWindow * window = (MyWindow *) d ;
    LOGD ("sss %d\n", window -> rack -> hearts.size ());
    LOGD ("Closing audio ...\n");
    window -> rack -> engine -> driver -> deactivate ();
    window -> rack -> engine -> driver -> close ();
    LOGD ("Saving preset ...\n");
    
    window -> rack -> engine -> savePreset (std::string (window -> presets -> dir) .append ("/default").c_str (), "Last saved preset") ;
    
    json favs = {} ;
    LOGD ("Saving favorites ...\n");
        
    for (int i = 0 ; i < window -> rack -> hearts.size (); i ++) {
        GtkWidget * fav = (GtkWidget *) window -> rack -> hearts.at (i);
        if (gtk_toggle_button_get_active ((GtkToggleButton *)fav)) {
            favs [gtk_widget_get_name (fav)] = true ;
        }
    }
    
    json_to_filename (favs, std::string (window -> presets -> dir).append ("/fav.json"));    

    LOGD ("Saving settings ...\n");
    # ifdef __linux__
    json_to_filename (window -> rack->config, std::string (getenv ("HOME")).append ("/.config/amprack/config.json"));    
    # else
    json_to_filename (window -> rack->config, std::string (getenv ("USERPROFILE")).append ("/.config/amprack/config.json"));    
    # endif
    
    LOGD ("Ok done, goodbye xoxo ...\n");

    gtk_window_destroy ((GtkWindow *)window -> gobj ());
    g_application_quit ((GApplication *)window -> app);
    OUT
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
    //~ printf ("[keypress] %d\n", keyval);
    Sync * sync ;
    switch (keyval) {
        case 65365:
            window -> rack -> next_preset ();
            break ;
        case 115: // 's'
            sync = new Sync (window -> rack);
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

void activate (GApplication * app, void * v) {
    IN
    //~ gtk_application_add_window ((GtkApplication *)app, window -> gobj ());
    GtkCssProvider *cssProvider = gtk_css_provider_new();
    GtkCssProvider *cssProvider2 = gtk_css_provider_new();
    gtk_css_provider_load_from_path(cssProvider, "style.css");

    MyWindow * window = new MyWindow ((GtkApplication *) app);
    gtk_widget_add_css_class ((GtkWidget *) window -> window, "xwindow");

	if ( std::filesystem::exists ("assets/themes/TubeAmp/style.css"))
		gtk_css_provider_load_from_path(cssProvider, std::string ("assets/themes/").append (window ->rack -> theme).append ("/style.css").c_str ());
	else
		gtk_css_provider_load_from_path(cssProvider, std::string ("/usr/share/amprack/assets/themes/").append (window ->rack -> theme).append ("/style.css").c_str ());
    gtk_style_context_add_provider_for_display (gdk_display_get_default (), (GtkStyleProvider *)cssProvider, GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

    # ifdef __linux__
    std::string user_css = std::string (getenv ("HOME")).append ("/.config/amprack/style.css").c_str () ;
    # else
    std::string user_css = std::string (getenv ("USERPROFILE")).append ("/.config/amprack/style.css").c_str () ;
    # endif
    
    if (std::filesystem::exists (user_css))
        gtk_css_provider_load_from_path(cssProvider2, user_css.c_str());
        
    gtk_style_context_add_provider_for_display (gdk_display_get_default (), (GtkStyleProvider *)cssProvider2, GTK_STYLE_PROVIDER_PRIORITY_USER);

    window ->rack -> engine -> processor -> bypass =  false ;

    gtk_window_present ((GtkWindow *)window ->window);
    LOGD ("we are live and rocking\n");
    OUT
}

void position_changed (GtkPaned * pane, void *) {
    printf ("[paned] %s\n", gtk_paned_get_position (pane)) ;
}

int main(int argc, char* argv[])
{
    LOGD ("Rock and roll can never die");
    IN
    # ifdef __linux__
    std::string config = std::string (getenv ("HOME")).append ("/.config/amprack/config.json");    
    # else
    std::string config = std::string (getenv ("USERPROFILE")).append ("/.config/amprack/config.json");    
    # endif
    
    LOGD ("[config] load: %s\n", config.c_str ());
    json j = filename_to_json (config);
    
    if (j.contains ("renderer")) {
        int renderer = j ["renderer"] . get <int> () ;
        if (renderer) {
            g_setenv ("GSK_RENDERER", renderers [renderer], 1);
            LOGD ("[config] set renderer to %s\n", renderers [renderer]);
        }
    } else {
        //  seems to be most stable *ahem* on win32
        # ifndef __linux__
        g_setenv ("GSK_RENDERER", "gl", 1);        
        # endif
    }

    auto app = gtk_application_new ("org.acoustixaudio.amprack", G_APPLICATION_DEFAULT_FLAGS);

    //~ window ->set_title("Gtk4 Demo");
    //~ window ->set_default_size(300 , 400);
    //~ g_signal_connect (app, "activate")
    //~ app->signal_activate().connect([&](){
        //~ app->add_window(window);
        //~ window ->show ();
    //~ });

    g_signal_connect (app, "activate", (GCallback) activate, NULL);
    return g_application_run ((GApplication *)app, argc ,argv);

  //~ GtkSettings * settings = gtk_settings_get_default ();
  //~ g_object_set (settings, "gtk-xft-dpi", 1.1, NULL);
    //~ return app->make_window_and_run<MyWindow>(argc, argv);
}

MyWindow * global_window_really_question_mark = nullptr;

void toggle_effects (GtkToggleButton * button, MyWindow * window) {
    GtkPaned * pane = window -> pane;
    
    if (gtk_toggle_button_get_active (button)) {
        gtk_paned_set_position (pane, 1600) ;
    } else {
        gtk_paned_set_position (pane, 1100) ;
    }

    /*
    GtkAdjustment * h = gtk_scrolled_window_get_hadjustment ((GtkScrolledWindow *) window -> rack -> sw.gobj ());
    GtkAdjustment * v = gtk_scrolled_window_get_vadjustment ((GtkScrolledWindow *) window -> rack -> sw.gobj ());
    printf ("[scroll] %d %d %d | %d %d %d\n",
        gtk_adjustment_get_lower (h),
        gtk_adjustment_get_upper (h),
        gtk_adjustment_get_value (h),
        gtk_adjustment_get_lower (v),
        gtk_adjustment_get_upper (v),
        gtk_adjustment_get_value (v));
    */
}

MyWindow::MyWindow(GtkApplication * _app)
{
    IN
    app = _app ;
    window = (GtkWindow *) gtk_application_window_new (app);
    gtk_window_set_title(window, "Amp Rack 5 (Alpha)");
    gtk_window_set_default_size(window, 700, 400);
    gtk_window_maximize (window);

    box = (GtkBox *) gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
    gtk_window_set_child (gobj (), (GtkWidget *)box);

    pane = (GtkPaned *)gtk_paned_new (GTK_ORIENTATION_HORIZONTAL);
    
    stack_box = (GtkBox *)gtk_box_new (GTK_ORIENTATION_VERTICAL, 10);
    
    gtk_box_append (box, (GtkWidget *)stack_box);
    
    gtk_paned_set_position (pane, 800);
    //~ g_signal_connect (pane.gobj (), "notify::position", (GCallback) position_changed, NULL);

    stack = (GtkStack *)gtk_stack_new ();
    gtk_box_append (stack_box, (GtkWidget *) stack);
    gtk_box_set_homogeneous (stack_box, false);

    switcher = (GtkStackSwitcher *)gtk_stack_switcher_new ();
    gtk_box_append (stack_box, (GtkWidget *)switcher);

    rack = new Rack () ;
    gtk_stack_add_child (stack, (GtkWidget *)pane);
    
    gtk_window_set_titlebar (gobj (), (GtkWidget *)rack->button_box);
    
    presets = new Presets () ;
    presets->_this = (void *) presets ;
    presets->engine = rack -> engine ;
    presets->rack = rack ;
    presets->app = app ;
    
    rack -> presets = (void *) presets;
    
    gtk_notebook_append_page (presets->notebook, rack->rack, gtk_label_new ("Effects"));
    
    presets->my () ;
    
    Settings settings = Settings (rack);
    gtk_notebook_append_page (presets->notebook, (GtkWidget *)settings . grid, gtk_label_new ("Settings"));
    
    
    CB * cb = new CB () ;
    cb -> engine = rack -> engine ;
    cb -> presets = presets ;
    
    g_signal_connect (presets->add, "clicked", (GCallback) save_preset_cb, cb);
    
    GtkScrolledWindow * sw = (GtkScrolledWindow *) gtk_scrolled_window_new ();
    gtk_scrolled_window_set_child (sw, (GtkWidget *)presets->master);
    
    gtk_paned_set_start_child (pane, (GtkWidget *)sw);
    gtk_paned_set_end_child (pane, (GtkWidget *)rack->master);
    g_signal_connect (this->gobj (), "close-request", (GCallback) quit, this);
    g_signal_connect (this->gobj (), "show", (GCallback) onshow, this);
 
    gtk_notebook_set_current_page (presets->notebook, 1); 
    
    GtkEventController *   event_controller = gtk_event_controller_key_new ();
    g_signal_connect_swapped (event_controller, "key-pressed",
                           G_CALLBACK (hotkeys),
                           this);
    gtk_widget_add_controller (GTK_WIDGET (gobj()), event_controller);
    
    rack -> hashCommands.emplace (std::make_pair (std::string ("quit"), &qquit));
    //~ g_signal_connect (rack -> toggle_presets.gobj (), "clicked", (GCallback) toggle_effects, this);
    //~ XWarpPointer(gdk_x11_display_get_xdisplay (gdk_display_get_default ()),0,0, 0, 0, 0, 0, 0, 
                //~ 0);
    OUT
}

GtkWindow * MyWindow::gobj () {
    return window ;
}
