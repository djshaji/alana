#ifndef JACK_H
#define JACK_H

class AudioDriver {
    jack_port_t *input_port;
    jack_port_t *output_port;
    jack_client_t *client;
    
    const char **ports;
	const char *client_name = "Amp Rack";
	const char *server_name = NULL;
	jack_options_t options = JackNullOption;
	jack_status_t status;
    
    bool open ();
    
} ;

#endif
