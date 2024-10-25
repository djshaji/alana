# ifndef __SYNC_H
# define __SYNC_H

# include "rack.h"
# include "defs.h"

class Sync {
    public:
    GtkWindow * window ;
    Rack * rack;
    
    Sync (Rack * w) ;
};

# endif
