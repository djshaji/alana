#ifndef PROCESS_H
#define PROCESS_H

#include <cstring>
#include <ladspa.h>
#include <cstdio>
#include "logging_macros.h"
#include "LockFreeQueue.h"

#define MAX_PLUGINS 10 // aaarrrrghhhhhh

class Processor {
public:
    void (*connect_port [MAX_PLUGINS])(LADSPA_Handle Instance,
                         unsigned long Port,
                         LADSPA_Data * DataLocation);
    void (*run [MAX_PLUGINS])(LADSPA_Handle Instance,
                unsigned long SampleCount);
    void (*run_adding [MAX_PLUGINS])(LADSPA_Handle Instance,
                unsigned long SampleCount);
    void (*set_run_adding_gain [MAX_PLUGINS])(LADSPA_Handle Instance,
                                LADSPA_Data   Gain);

    void * handle [MAX_PLUGINS] ;
    LADSPA_Data run_adding_gain [MAX_PLUGINS] ;
    const LADSPA_Descriptor * descriptor [MAX_PLUGINS] ;
    int inputPorts [MAX_PLUGINS] ;
    int inputPorts2 [MAX_PLUGINS] ;
    int outputPorts [MAX_PLUGINS] ;
    int outputPorts2 [MAX_PLUGINS] ;

    int activePlugins = 0;
    void process (int, float *, float *);
    bool bypass = false, recording = false ;
    LockFreeQueueManager * lockFreeQueueManager;

};
#endif
