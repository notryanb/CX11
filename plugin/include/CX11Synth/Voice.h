#pragma once

#include "Oscillator.h"
#include "Envelope.h"

struct Voice {
    int note;
    float saw;
    Envelope env;

    Oscillator osc;

    void reset() {
        note = 0;
        saw = 0.0f;
        osc.reset();
    }

    float render(float input) {
        // 0.997f is a "leaky" integrator. Acts as a LPF that prevents an offset from building up.
        float sample = osc.next_sample();
        saw = saw * 0.997f + sample;
         
        float output = saw + input; // mixes in the noise
        float envelope = env.nextValue(); // apply the envlope
        return output * envelope;
    }
};