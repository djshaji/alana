#ifndef SND_H
#define SND_H

#include <stdio.h>
#include <sndfile.h>
#include <cstdlib>
#include "log.h"

class SoundFile {
    public:
    int len ;
    float * data ;
    
    SoundFile (int _len) {
        len = _len ;
        data = (float *) malloc (sizeof (float) * len + 1);
    }
    
    ~SoundFile () {
        free (data) ;
    }
};

SoundFile * snd_read (char * filename);

#endif
