# ifndef __SYNC_H
# define __SYNC_H

# include "rack.h"
# include "defs.h"
# include "server.h"
# include "util.h"
# include "client.h"

class Sync {
    public:
    GtkEntry * entry ;
    Server * server ;
    GtkWindow * window ;
    std::thread * t;
    Rack * rack;
    int port ;
    Sync (Rack * w) ;
};

void run_server (Sync * sync) ;

# endif
