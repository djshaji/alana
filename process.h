#ifndef PROCESS_H
#define PROCESS_H

#include <cstring>
#include <ladspa.h>
#include <cstdio>
#include "logging_macros.h"
#include "LockFreeQueue.h"

# ifndef __linux__
# include <windows.h>
# endif

#define MAX_PLUGINS 10 // aaarrrrghhhhhh

class Processor {
public:
    static void (*connect_port [MAX_PLUGINS])(LADSPA_Handle Instance,
                         unsigned long Port,
                         LADSPA_Data * DataLocation);
    static void (*run [MAX_PLUGINS])(LADSPA_Handle Instance,
                unsigned long SampleCount);
    static void (*run_adding [MAX_PLUGINS])(LADSPA_Handle Instance,
                unsigned long SampleCount);
    static void (*set_run_adding_gain [MAX_PLUGINS])(LADSPA_Handle Instance,
                                LADSPA_Data   Gain);

    static void * handle [MAX_PLUGINS] ;
    LADSPA_Data run_adding_gain [MAX_PLUGINS] ;
    const LADSPA_Descriptor * descriptor [MAX_PLUGINS] ;
    static int inputPorts [MAX_PLUGINS] ;
    static int inputPorts2 [MAX_PLUGINS] ;
    static int outputPorts [MAX_PLUGINS] ;
    static int outputPorts2 [MAX_PLUGINS] ;

    static int activePlugins;
    void process (int, float *, float *);
    static bool bypass, recording;
    static LockFreeQueueManager * lockFreeQueueManager;
    
    Processor () ;
};
#endif
