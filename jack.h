#ifndef JACK_H
#define JACK_H

#include <jack/jack.h>
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
    
    jack_port_t *input_port;
    jack_port_t *output_port;
    jack_client_t *client;
    
    const char **i_ports, **o_ports;
	const char *client_name = "Amp Rack";
	const char *server_name = NULL;
	jack_options_t options = JackNullOption;
	jack_status_t status;
    
    bool open ();
    void close ();
    int get_sample_rate () ;
    bool activate () ;
    bool deactivate () ;
    int get_buffer_size ();
} ;

#endif
