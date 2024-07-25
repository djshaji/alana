#ifndef SND_H
#define SND_H

#include <stdio.h>
#include <sndfile.h>
#include <cstdlib>
#include "log.h"

class SoundFile {
    public:
    float * data ;
    int * len ;
    
    SoundFile (int _len) {
        len = (int *) malloc (sizeof (int));
        * len = _len ;
        data = (float *) malloc (sizeof (float) * *len + 1);
    }
    
    ~SoundFile () {
        free (data) ;
        free (len);
    }
};

SoundFile * snd_read (char * filename);

#endif
