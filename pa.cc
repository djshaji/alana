#include "pa.h"

int
process (int nframes, void *arg)
{
    AudioDriver * driver = (AudioDriver *) arg ;
    float *in, *out;
	
    driver -> processor -> process (nframes, in, out);
    return 0;      
}

bool AudioDriver::activate () {

	/* Connect the ports.  You can't do this before the client is
	 * activated, because we can't make connections to clients
	 * that aren't running.  Note the confusing (but necessary)
	 * orientation of the driver backend ports: playback ports are
	 * "input" to the backend, and capture ports are "output" from
	 * it.
	 */

    return true ;
}

bool AudioDriver::deactivate () {
    
    return true ;

}

bool AudioDriver::open () {
    return activate () ;
}

void AudioDriver::close () {

}

int AudioDriver::get_sample_rate () {
    return 48000;
}

int AudioDriver::get_buffer_size () {
    return 192 ;
}
