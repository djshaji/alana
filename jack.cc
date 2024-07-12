#include "jack.h"

bool AudioDriver::open () {
	client = jack_client_open (client_name, options, &status, server_name);
    if (client == NULL) {
        fprintf (stderr, "jack_client_open() failed, "
             "status = 0x%2.0x\n", status);

        if (status & JackServerFailed) {
            fprintf (stderr, "Unable to connect to JACK server\n");
            
        return false ;
    }

	if (status & JackServerStarted) {
		fprintf (stderr, "JACK server started\n");
	}

	if (status & JackNameNotUnique) {
		client_name = jack_get_client_name(client);
		fprintf (stderr, "unique name `%s' assigned\n", client_name);
	}

    
    return true ;
}



