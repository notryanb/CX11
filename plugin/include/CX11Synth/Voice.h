#pragma once

#include "Oscillator.h"

struct Voice {
    int note;
    float saw;

    Oscillator osc;

    void reset() {
        note = 0;
        saw = 0.0f;
        osc.reset();
    }

    float render() {
        // 0.997f is a "leaky" integrator. Acts as a LPF that prevents an offset from building up.
        float sample = osc.next_sample();
        saw = saw * 0.997f + sample;
        return saw;
    }
};