#include "jack.h"

int
process (jack_nframes_t nframes, void *arg)
{
    AudioDriver * driver = (AudioDriver *) arg ;
	jack_default_audio_sample_t *in, *out;
	
	in = (float *)jack_port_get_buffer (driver -> input_port, nframes);
	out = (float *)jack_port_get_buffer (driver -> output_port, nframes);
	
    driver -> processor -> process (nframes, in, out);

	return 0;      
}

/**
 * JACK calls this shutdown_callback if the server ever shuts down or
 * decides to disconnect the client.
 */
void
jack_shutdown (void *arg)
{
	exit (1);
}

bool AudioDriver::open () {
	client = jack_client_open (client_name, options, &status, server_name);
    if (client == NULL) {
        fprintf (stderr, "jack_client_open() failed, "
             "status = 0x%2.0x\n", status);
        return false ;
    }

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

    jack_set_process_callback (client, process, this);    
	jack_on_shutdown (client, jack_shutdown, 0);

	printf ("engine sample rate: %" PRIu32 "\n",
		jack_get_sample_rate (client));

	/* create two ports */

	input_port = jack_port_register (client, "input",
					 JACK_DEFAULT_AUDIO_TYPE,
					 JackPortIsInput, 0);
	output_port = jack_port_register (client, "output",
					  JACK_DEFAULT_AUDIO_TYPE,
					  JackPortIsOutput, 0);

	if ((input_port == NULL) || (output_port == NULL)) {
		fprintf(stderr, "no more JACK ports available\n");
		exit (1);
	}

	/* Tell the JACK server that we are ready to roll.  Our
	 * process() callback will start running now. */

	if (jack_activate (client)) {
		fprintf (stderr, "cannot activate client");
		return false ;
	}

	/* Connect the ports.  You can't do this before the client is
	 * activated, because we can't make connections to clients
	 * that aren't running.  Note the confusing (but necessary)
	 * orientation of the driver backend ports: playback ports are
	 * "input" to the backend, and capture ports are "output" from
	 * it.
	 */

	i_ports = jack_get_ports (client, NULL, NULL,
				JackPortIsPhysical|JackPortIsOutput);
	if (i_ports == NULL) {
		fprintf(stderr, "no physical capture ports\n");
		return false ;
	}

	if (jack_connect (client, i_ports[0], jack_port_name (input_port))) {
		fprintf (stderr, "cannot connect input ports\n");
	}

	o_ports = jack_get_ports (client, NULL, NULL,
				JackPortIsPhysical|JackPortIsInput);
	if (o_ports == NULL) {
		fprintf(stderr, "no physical playback ports\n");
		return false ;
	}

	if (jack_connect (client, jack_port_name (output_port), o_ports[0])) {
		fprintf (stderr, "cannot connect output ports\n");
        return false ;
	}

    LOGD ("[audio engine ok]");
    free (i_ports);
	free (o_ports);

    return true ;
}

void AudioDriver::close () {
	jack_client_close (client);    
}
