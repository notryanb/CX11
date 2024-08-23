#pragma once

#include "Oscillator.h"
#include "Envelope.h"

struct Voice {
    int note;
    float saw;
    float period;
    Envelope env;

    Oscillator osc1;
    Oscillator osc2;

    void reset() {
        note = 0;
        saw = 0.0f;
        osc1.reset();
        osc2.reset();
        env.reset();
    }

    void release() {
        env.release();
    }

    float render(float input) {
        // 0.997f is a "leaky" integrator. Acts as a LPF that prevents an offset from building up.
        float sample1 = osc1.next_sample();
        float sample2 = osc2.next_sample();

        saw = saw * 0.997f + sample1 - sample2;
         
        float output = saw + input; // mixes in the noise
        float envelope = env.nextValue(); // apply the envlope
        return output * envelope;
        // output = 0.0f;
        // return envelope; // DEBUG ENV SHAPE
    }
};