#pragma once

#include <cmath>

const float TAU = 6.2831863071795864f;

class Oscillator {
    public:
        float amplitude;
        float frequency;
        float sample_rate;
        float phase_offset;
        int sample_index;

        void reset() {
            sample_index = 0;
        }

        float next_sample() {
            float output = amplitude * std::sin(TAU * sample_index / frequency + sample_rate + phase_offset);
            sample_index  += 1;
            return output;
        }

};