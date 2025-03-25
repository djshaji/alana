# ifndef __linux__1
#define CPPHTTPLIB_OPENSSL_SUPPORT
# include "httplib.h"
# endif
#include "util.h"

json filename_to_json (std::string filename) {
    //~ IN
    if (! std::filesystem::exists (filename)) {
        HERE LOGD ("%s does not exist!\n", filename.c_str ());
        filename = std::string ("/usr/share/amprack/").append (filename);
        if (! std::filesystem::exists (filename)) {
            HERE LOGD ("file does not exist! %s\n", filename.c_str ()) ;
            // instead of null, empty json is better
            return json::parse ("{}");
        }
    }
    
    std::ifstream fJson(filename);
    std::stringstream buffer;
    buffer << fJson.rdbuf();
    //~ LOGD ("reading file %s\n%s\n", filename.c_str (), buffer.str ());
    auto j = json::parse(buffer.str ());
    //~ OUT
    return j ;
}

bool json_to_filename (json j, std::string filename) {
    LOGV (filename.c_str ());
    LOGD ("%s -> %s\n", filename.c_str (), j.dump ().c_str ());
    std::ofstream o(filename);
    o << std::setw(4) << j << std::endl;
    return true;
}

void alert_yesno (std::string title, std::string msg, GAsyncReadyCallback cb, gpointer data) {
    IN
    LOGV (msg.c_str ());
    /*
    GtkAlertDialog *dialog = gtk_alert_dialog_new (msg.c_str ());
    const char* buttons[] = {"Cancel", "Ok", NULL};
    gtk_alert_dialog_set_detail (dialog, msg.c_str ());
    gtk_alert_dialog_set_buttons (dialog, buttons);
    gtk_alert_dialog_set_cancel_button (dialog, 0);   // Cancel is at index 0
    gtk_alert_dialog_set_default_button (dialog, 1);  // If the user presses enter key, "Proceed" button

    GListModel * list = gtk_window_get_toplevels () ;
    
    gtk_alert_dialog_set_modal (dialog, false);
    gtk_window_set_default_size ((GtkWindow *)dialog, 400,300);
    gtk_window_maximize ((GtkWindow *) dialog);
    //~ gtk_alert_dialog_show(dialog, null);
    gtk_alert_dialog_choose (dialog, (GtkWindow *)g_list_model_get_item (list, 0), null, cb, data);
    * */
    OUT
}

void alert (char * title, char * msg, AlertType type, gpointer callback, gpointer data) {
    GtkDialogFlags flags = (GtkDialogFlags) (GTK_DIALOG_DESTROY_WITH_PARENT | GTK_DIALOG_MODAL);
    GtkWidget * dialog = (GtkWidget *) gtk_message_dialog_new (null,
                                     (GtkDialogFlags)flags,
                                     GTK_MESSAGE_INFO,
                                     GTK_BUTTONS_OK_CANCEL,
                                     title,
                                     msg,
                                     null);
    // Destroy the dialog when the user responds to it
    // (e.g. clicks a button)

    Alert_CB * cb = new Alert_CB () ;
    cb -> data = data ;

    GtkWidget * box = (GtkWidget *)gtk_box_new (GTK_ORIENTATION_VERTICAL, 10) ;
    gtk_box_append ((GtkBox *)gtk_dialog_get_content_area ((GtkDialog *)dialog), box);
    
    GtkWidget * label = gtk_label_new (msg);
    gtk_box_append ((GtkBox *)gtk_message_dialog_get_message_area ((GtkMessageDialog *)dialog), label);
    
    GtkWidget * entry = gtk_entry_new ();
    gtk_box_append ((GtkBox *)box, entry);    

    cb -> widget = entry ;
    cb -> dialog = dialog ;
    g_signal_connect (dialog, "response",
                      G_CALLBACK (callback),
                      cb);
    
    gtk_widget_set_size_request (dialog, 400, 300);
    gtk_window_present ((GtkWindow *) dialog);
}

void msg (std::string message) {
    LOGD ("%s\n", message.c_str ()); 
    #ifdef __GTK_ALERT_DIALOG_H__
    GtkAlertDialog * dialog = gtk_alert_dialog_new (message.c_str());
    gtk_alert_dialog_show (dialog, null);
    #endif
}

# ifdef __linux__1
bool download_file (char *name, const char * filename) {
    IN
    LOGD ("[download] %s -> %s\n", name, filename);
    GFile *f = g_file_new_for_uri(name);
    GFileInputStream *fis = NULL;
    GDataInputStream* dis = NULL;
    GError *err = NULL;
    //char buffer[2048];
    char *buffer;
    size_t length;
    bool ret = false;

    GFileInfo *info;

    int total_size = -1;

    /* get input stream */
    fis = g_file_read(f, NULL, &err);

    if (err != NULL) {
        LOGD("ERROR: opening %s\n", name);
        g_object_unref(f);
        OUT
        return false;
    } else {
        LOGV ("file read ok");
        
    }

    info = g_file_input_stream_query_info (G_FILE_INPUT_STREAM (fis),G_FILE_ATTRIBUTE_STANDARD_SIZE,NULL, &err);
    if (info)
    {
        if (g_file_info_has_attribute (info, G_FILE_ATTRIBUTE_STANDARD_SIZE)) {
            total_size = g_file_info_get_size (info);
            LOGD( "total_size = %d\n", total_size);
            g_object_unref (info);
        } else {
            LOGV ("no attribute info on file!");
        }
    } else {
        LOGD ("file input query info failed! [%d] %s\n", err -> code, err -> message);
    }

    // fill buffer
    if(total_size > 0){
        FILE * fd = fopen (filename, "w");
        buffer = (char *) malloc(sizeof(char) * total_size);
        memset(buffer, 0, total_size);
        int i = 0 ;
        while  ((length = g_input_stream_read (G_INPUT_STREAM(fis),
                    buffer, total_size, NULL, &err)) != -1 && i < total_size) {
                LOGD( "%s\n", buffer);
            fwrite (buffer, length, 1, fd);
            LOGD ("%d/%d\n", i, total_size);
            i += length ;
        }
        
        fclose (fd);
        ret = true;
    } else {
        LOGV ("error: total size < 0");
    }
    // close streams
    g_object_unref(fis);
    g_object_unref(f);   
    OUT
    return ret;
}
# else
bool download_file (char *name, const char * filename) {
    IN
    httplib::Client cli("http://amprack.in");
    LOGV ("client init ok, getting file ...");
    auto res = cli.Get ("/presets.json");
    LOGD ("[download] %s\n[%d] -> %s\n", name, res -> status, res -> body.c_str ());
    std::ofstream out (filename);
    out << res -> body ;
    out.close ();
    OUT
    return true;
}

# endif
void copy_file (std::string source, std::string dest) {
    if (std::filesystem::exists (dest)) {
        wtf ("%s already exists, not overwriting\n", dest.c_str ());
        return ;
    }
    
    wtf ("copying %s -> %s\n", source.c_str (), dest.c_str ());
    std::ifstream  src(source.c_str (), std::ios::binary);
    std::ofstream  dst(dest.c_str (),   std::ios::binary);

    dst << src.rdbuf();
}


char ** list_directory (std::string dir) {
    IN
    wtf ("[dir] %s\n", dir.c_str ());
    if (! std::filesystem::exists (dir)) {
        char ** entries = (char **) malloc (1) ;
        entries [0] = NULL ;
        return entries ;
    }
        
    std::vector <std::filesystem::path> files ;
    for (const auto & entry : std::filesystem::directory_iterator(dir)) {
        //~ wtf ("[file] %s\n", entry.path ());
        files.push_back (entry.path ());
    }
    
    char ** entries = (char **)malloc (files.size () + 1);
    for (int i = 0 ; i < files.size (); i ++) {
        //~ std::string path = std::string (files.at (i)) ;
        std::string path {files.at (i).string ()} ;
        //~ wtf ("[before] %s\n", path);
        path = path.substr(path.find_last_of("/") + 1).c_str () ;
        //~ wtf ("[after] %s\n", path);
        entries [i] = strdup (path.c_str ());
    }
    
    entries [files.size ()] = NULL;
    OUT
    return entries;
}

void set_random_background (GtkWidget * widget) {
    IN
    #ifdef __GTK_ALERT_DIALOG_H__

    # ifdef __linux__
    std::string dir = std::string (getenv ("HOME")).append ("/amprack/backgrounds");
    # else
    std::string dir = std::string (getenv ("USERPROFILE")).append ("/amprack/backgrounds");    
    # endif
    
    if (! std::filesystem::exists (dir))
        return ;

    std::vector <std::string> files ;
    for (const auto & entry : std::filesystem::directory_iterator(dir)) {
        files.push_back (entry.path ());
    }

    std::string filename ;
    int index = rand () % files.size () ;
    
    filename = files.at (index);
    wtf ("[random] filename: %s\n", filename.c_str ());

    GtkCssProvider *cssProvider = gtk_css_provider_new();
    std::string css = 
        std::string ("#plugin {background-image:url (\"file://")
        .append (filename)
        .append ("\");background-repeat: no-repeat;background-size: cover;}");

    wtf ("[random] css: %s\n", css.c_str ());

    gtk_css_provider_load_from_string(cssProvider, css.c_str ());
    //~ gtk_style_context_add_provider (gtk_widget_get_style_context (widget), (GtkStyleProvider *)cssProvider, GTK_STYLE_PROVIDER_PRIORITY_USER) ;
    gtk_style_context_add_provider_for_display (gdk_display_get_default (), (GtkStyleProvider *)cssProvider, GTK_STYLE_PROVIDER_PRIORITY_USER);
    #endif
}
