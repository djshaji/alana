#include "util.h"

json filename_to_json (std::string filename) {
    std::ifstream fJson(filename);
    std::stringstream buffer;
    buffer << fJson.rdbuf();
    auto j = json::parse(buffer.str ());
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
