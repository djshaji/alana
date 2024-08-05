#include "util.h"

json filename_to_json (std::string filename) {
    //~ IN
    if (! std::filesystem::exists (filename)) {
        filename = std::string ("/usr/share/amprack/").append (filename);
        if (! std::filesystem::exists (filename)) {
            HERE LOGD ("file does not exist! %s\n", filename.c_str ()) ;
            // instead of null, empty json is better
            return json {} ;
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
    std::ofstream o(filename);
    o << std::setw(4) << j << std::endl;
    return true;
}

void alert_yesno (std::string title, std::string msg, GAsyncReadyCallback cb, gpointer data) {
    IN
    LOGV (msg.c_str ());
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
    OUT
}

void alert (char * title, char * msg, AlertType type, gpointer callback, gpointer data) {
    GtkDialogFlags flags = (GtkDialogFlags) (GTK_DIALOG_DESTROY_WITH_PARENT | GTK_DIALOG_MODAL);
    GtkWidget * dialog = gtk_message_dialog_new (null,
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
    GtkAlertDialog * dialog = gtk_alert_dialog_new (message.c_str());
    gtk_alert_dialog_show (dialog, null);
}

bool download_file (char *name, const char * filename) {
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
        fprintf(stderr, "ERROR: opening %s\n", name);
        g_object_unref(f);
        return false;
    }

    info = g_file_input_stream_query_info (G_FILE_INPUT_STREAM (fis),G_FILE_ATTRIBUTE_STANDARD_SIZE,NULL, &err);
    if (info)
    {
        if (g_file_info_has_attribute (info, G_FILE_ATTRIBUTE_STANDARD_SIZE))
            total_size = g_file_info_get_size (info);
            printf( "total_size = %d\n", total_size);
            g_object_unref (info);
    }

    // fill buffer
    if(total_size > 0){
        FILE * fd = fopen (filename, "w");
        buffer = (char *) malloc(sizeof(char) * total_size);
        memset(buffer, 0, total_size);
        int i = 0 ;
        while  ((length = g_input_stream_read (G_INPUT_STREAM(fis),
                    buffer, total_size, NULL, &err)) != -1 && i < total_size) {
                //~ printf( "%s\n", buffer);
            fwrite (buffer, length, 1, fd);
            //~ printf ("%d/%d\n", i, total_size);
            i += length ;
        }
        
        fclose (fd);
        ret = true;
    }
    // close streams
    g_object_unref(fis);
    g_object_unref(f);   
    return ret;
}
