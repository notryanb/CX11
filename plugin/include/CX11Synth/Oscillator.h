#pragma once

#include <cmath>

const float TAU = 6.2831863071795864f;

class Oscillator {
    public:
        float amplitude;
        float phase_inc;
        float phase;

        void reset() {
            phase = 1.5707963268f;
        }

        float next_sample() {
            phase += phase_inc;
            if (phase >= 1.0) {
                phase -= 1.0;
            }

            return amplitude * std::sin(TAU * phase);
        }

};