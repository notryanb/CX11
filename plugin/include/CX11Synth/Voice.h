#pragma once

#include <algorithm>
#include "Oscillator.h"
#include "Envelope.h"

struct Voice {
    int note;
    float saw;
    float period;
    float pan_left, pan_right;
    Envelope env;

    Oscillator osc1;
    Oscillator osc2;

    void reset() {
        note = 0;
        saw = 0.0f;
        pan_left = 0.707f;
        pan_right = 0.707f;
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

    // TODO - lookup Haas effect / comb filtering  re: stereo widening
    void updatePanning() {
        float panning = std::clamp((note - 60.0f) / 24.0f, -1.0f, 1.0f);
        pan_left = std::sin(PI_OVER_4 * (1.0f - panning));
        pan_right = std::sin(PI_OVER_4 * (1.0f + panning));
    }
};