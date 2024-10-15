# include <gtk/gtk.h>
# ifndef __linux__
# include <windows.h>

int printf_windows (const char* format, ...) ;
int printf_windows (const char* format, ...) {
    char * msg = g_strdup_printf (format);
    //~ OutputDebugStringA (msg);
    g_log_default_handler ("[amprack]", G_LOG_LEVEL_WARNING, msg, NULL);
    g_free (msg);
    return 0 ;
}

# endif
