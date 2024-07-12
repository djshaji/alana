#include "process.h"

void Processor::process (int n_samples, float * in, float * out) {
    memcpy (out, in,
		sizeof (float) * n_samples);
}

