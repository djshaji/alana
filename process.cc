#include "process.h"

bool Processor::recording = false ;
bool Processor::bypass = false ;
int Processor::activePlugins = 0;
int Processor::inputPorts [MAX_PLUGINS];
int Processor::inputPorts2 [MAX_PLUGINS];
int Processor::outputPorts [MAX_PLUGINS];
int Processor::outputPorts2 [MAX_PLUGINS];
void * Processor::handle [MAX_PLUGINS] ;
LockFreeQueueManager * Processor::lockFreeQueueManager;

void (*Processor::connect_port [MAX_PLUGINS])(LADSPA_Handle Instance,
                     unsigned long Port,
                     LADSPA_Data * DataLocation);
void (*Processor::run [MAX_PLUGINS])(LADSPA_Handle Instance,
            unsigned long SampleCount);
void (*Processor::run_adding [MAX_PLUGINS])(LADSPA_Handle Instance,
            unsigned long SampleCount);
void (*Processor::set_run_adding_gain [MAX_PLUGINS])(LADSPA_Handle Instance,
                            LADSPA_Data   Gain);



void Processor::process (int n_samples, float * in, float * data) {
    memcpy (data, in, sizeof (float) * n_samples);
    //~ LOGD ("[process] %d\n", GetCurrentThreadId());
    if (bypass) {
        //~ LOGD ("[status: %d] WARNING: audio driver process bypass\n", bypass);
        return ;
    }
    
    //~ LOGD ("active plugins: %d", activePlugins);
    
    for (int i = 0 ; i < activePlugins ; i ++) {
        //~ LOGD ("[process plugin] %d", i);
        if (inputPorts [i] != -1)
            connect_port [i] (handle [i], inputPorts [i], (LADSPA_Data *) data);
        if (outputPorts [i] != -1)
            connect_port [i] (handle [i], outputPorts [i], (LADSPA_Data *) data);

        if (inputPorts2 [i] != -1)
            connect_port [i] (handle [i], inputPorts2 [i], (LADSPA_Data *) data);
        if (outputPorts2 [i] != -1)
            connect_port [i] (handle [i], outputPorts2 [i], (LADSPA_Data *) data);

        if (run [i] == NULL)
            LOGD ("run %d is null", i);
        else
            run [i] (handle [i], n_samples);
        /*
        if (set_run_adding_gain [i] != NULL)
            set_run_adding_gain [i] (handle [i], run_adding_gain [i]) ;
        if (run_adding [i] != NULL)
            run [i] (handle [i], samplesToProcess);
        */

    }

    //~ if (recording)
    lockFreeQueueManager->process(in, data, n_samples) ;
}

Processor::Processor () {
    bypass = false ;
    recording = false ;
    LOGD ("bypass: %d\n", bypass);
}
