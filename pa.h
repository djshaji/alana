#ifndef PA_H
#define PA_H

#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <portaudio.h>
#include <glib.h>

#include "log.h"
#include "process.h"

#define SAMPLE_RATE  (48000)
#define FRAMES_PER_BUFFER (1024)
#define NUM_CHANNELS    (2)
#define NUM_SECONDS     (15)
/* #define DITHER_FLAG     (paDitherOff)  */
#define DITHER_FLAG     (0) /**/
#define PA_SAMPLE_TYPE  paFloat32
#define SAMPLE_SIZE (4)

class AudioDriver {
public:    
    PaStreamParameters inputParameters, outputParameters;
    PaStream *stream = NULL;
    PaError err;
    char sampleBlock [8192] ;
    int i;
    int numBytes;
    
    Processor * processor = nullptr;
    AudioDriver (Processor * e) {
        processor = e ;
    }
       
    bool open ();
    void close ();
    int get_sample_rate () ;
    bool activate () ;
    bool deactivate () ;
    int get_buffer_size ();
    void pa_error ();
} ;

#endif
