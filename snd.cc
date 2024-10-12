#ifdef __linux__
#include "snd.h"

SoundFile * snd_read (char * filename) {
    IN
    SF_INFO info ;
    info.format = 0 ;
    
    SNDFILE * sndfile = sf_open (filename, SFM_READ, &info);
    LOGD ("opening file %s: %d frames\n", filename, info.frames);
    if (sndfile == NULL) {
        LOGD ("cannot open file [%s]: %s\n", sf_strerror (NULL));
        OUT
        return NULL ;
    }
    
    SoundFile * soundFile = new SoundFile (info.frames);
    int val = sf_readf_float (sndfile, soundFile->data, info.frames);
    if (val != info.frames) {
        LOGD ("file read mismatch! total: %d\tread: %d\n", val, info.frames);
    } else {
        LOGD ("read %d bytes\n", val);
    }
    
    sf_close (sndfile);
    OUT
    return soundFile ;
}
#endif
