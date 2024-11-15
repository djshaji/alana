# ifndef __SYNC_H
# define __SYNC_H

# include "rack.h"
//# include "defs.h"

# ifdef __linux__
    # include "server.h"
    # include "client.h"
# else
    # include "winserver.h"
# endif

# include "util.h"
# include "random"

class Sync {
    public:
    GtkEntry * entry ;
    Server * server ;
    GtkLabel * status ;
    GtkLabel * header ;
    GtkWindow * window ;
    std::thread * t;
    Rack * rack;
    int port ;
    int sec_key ;
    Sync (Rack * w) ;
};

void run_server (Sync * sync) ;

# endif
