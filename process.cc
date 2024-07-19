#include "process.h"

void Processor::process (int n_samples, float * in, float * data) {
    memcpy (data, in, sizeof (float) * n_samples);
    if (bypass)
        return ;
        
    for (int i = 0 ; i < activePlugins ; i ++) {
        if (inputPorts [i] != -1)
            connect_port [i] (handle [i], inputPorts [i], (LADSPA_Data *) data);
        if (outputPorts [i] != -1)
            connect_port [i] (handle [i], outputPorts [i], (LADSPA_Data *) data);

        if (inputPorts2 [i] != -1)
            connect_port [i] (handle [i], inputPorts2 [i], (LADSPA_Data *) data);
        if (outputPorts2 [i] != -1)
            connect_port [i] (handle [i], outputPorts2 [i], (LADSPA_Data *) data);

        if (run [i] == NULL)
            LOGF ("run %d is null", i);
        else
            run [i] (handle [i], n_samples);
        /*
        if (set_run_adding_gain [i] != NULL)
            set_run_adding_gain [i] (handle [i], run_adding_gain [i]) ;
        if (run_adding [i] != NULL)
            run [i] (handle [i], samplesToProcess);
        */

    }

}

