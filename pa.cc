#include "pa.h"

void AudioDriver::pa_error () {
    fprintf( stderr, "An error occured while using the portaudio stream\n" );
    fprintf( stderr, "Error number: %d\n", err );
    fprintf( stderr, "Error message: %s\n", Pa_GetErrorText( err ) );    
}

int
process (int nframes, void *arg)
{
    AudioDriver * driver = (AudioDriver *) arg ;
    float *in, *out;
	
    //~ driver -> err = Pa_ReadStream( driver -> stream, driver -> sampleBlock, FRAMES_PER_BUFFER );
    //~ driver -> err = Pa_WriteStream( driver -> stream, driver -> sampleBlock, FRAMES_PER_BUFFER );
    //~ driver -> processor -> process (nframes, in, out);
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


    inputParameters.device = Pa_GetDefaultInputDevice(); /* default input device */
    printf( "Input device # %d.\n", inputParameters.device );
    printf( "Input LL: %g s\n", Pa_GetDeviceInfo( inputParameters.device )->defaultLowInputLatency );
    printf( "Input HL: %g s\n", Pa_GetDeviceInfo( inputParameters.device )->defaultHighInputLatency );
    inputParameters.channelCount = NUM_CHANNELS;
    inputParameters.sampleFormat = PA_SAMPLE_TYPE;
    inputParameters.suggestedLatency = Pa_GetDeviceInfo( inputParameters.device )->defaultHighInputLatency ;
    inputParameters.hostApiSpecificStreamInfo = NULL;

    outputParameters.device = Pa_GetDefaultOutputDevice(); /* default output device */
    printf( "Output device # %d.\n", outputParameters.device );
    printf( "Output LL: %g s\n", Pa_GetDeviceInfo( outputParameters.device )->defaultLowOutputLatency );
    printf( "Output HL: %g s\n", Pa_GetDeviceInfo( outputParameters.device )->defaultHighOutputLatency );
    outputParameters.channelCount = NUM_CHANNELS;
    outputParameters.sampleFormat = PA_SAMPLE_TYPE;
    outputParameters.suggestedLatency = Pa_GetDeviceInfo( outputParameters.device )->defaultHighOutputLatency;
    outputParameters.hostApiSpecificStreamInfo = NULL;

    /* -- setup -- */

   err = Pa_OpenStream(
              &stream,
              &inputParameters,
              &outputParameters,
              SAMPLE_RATE,
              FRAMES_PER_BUFFER,
              paClipOff,      /* we won't output out of range samples so don't bother clipping them */
              NULL, /* no callback, use blocking API */
              NULL ); /* no callback, so no callback userData */
    if( err != paNoError ) {
	HERE LOGD ("[fail] Pa_OpenStream\n");
	pa_error ();
	return false ;
    }

    err = Pa_StartStream( stream );
    if( err != paNoError ) {
	HERE LOGD ("[fail] Pa_StartStream\n");
	pa_error ();
	return false;
    }

    LOGD("portaudio init success\n");
    return true ;
}

bool AudioDriver::deactivate () {
    err = Pa_StopStream( stream );
    if( err != paNoError ) {
	HERE LOGD ("[fail] Pa_StopStream\n");
	pa_error ();
	return false;
    }

    Pa_CloseStream( stream );
    return true ;

}

bool AudioDriver::open () {
    IN
    numBytes = FRAMES_PER_BUFFER * NUM_CHANNELS * SAMPLE_SIZE ;
    //~ LOGD ("allocating memory: %d\n", numBytes);
    //~ char * s = (char *) malloc (numBytes);
    //~ char * sampleBlock = (char *) malloc( numBytes );
    //~ malloc (numBytes);
    //~ LOGD ("memory allocated! great success!\n");
    //~ if( sampleBlock == NULL )
    //~ {
        //~ printf("Could not allocate record array.\n");
	//~ OUT
        //~ exit(1);
    //~ }

    err = Pa_Initialize();
    if( err != paNoError ) {
	HERE LOGD ("[fail] Pa_Initialize\n");
	pa_error ();
	return false;
    }

    LOGD( "PortAudio version number = %d\nPortAudio version text = '%s'\n",
            Pa_GetVersion(), Pa_GetVersionText() );

    OUT
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
