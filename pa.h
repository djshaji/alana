#ifndef PA_H
#define PA_H

#include <cstdlib>
#include <cstdio>
#include <cstring>

#include "log.h"
#include "process.h"

class AudioDriver {
public:    
    Processor * processor = nullptr;
    AudioDriver (Processor * e) {
        processor = e ;
    }
       
    bool open ();
    void close ();
    int get_sample_rate () ;
    bool activate () ;
    bool deactivate () ;
    int get_buffer_size ();
} ;

#endif
