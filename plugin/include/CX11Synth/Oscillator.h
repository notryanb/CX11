#pragma once

#include <cmath>

const float PI_OVER_4 = 0.7853981633974483f;
const float PI = 3.1415926535897932f;
const float TAU = 6.2831863071795864f;

class Oscillator {
    public:
        float amplitude = 1.0f;
        float period = 0.0f;

        // float freq;
        // float sample_rate;
        // float phase_bl;

        void reset() {
            phase_inc = 0.0f;
            phase = 0.0f;
            dc_offset = 0.0f;

            sin0 = 0.0f;
            sin1 = 0.0f;
            dsin = 0.0f;
        }

        float next_sample() {
            // BLIT = Bandlimited Impulse Train
            float output = 0.0f;
            phase += phase_inc; // phase measured in samples * PI

            // Start new impulse when phase is less than PI/4 (45 deg?).
            if (phase <= PI_OVER_4) {
                // Find midpoint between impulse peaks. If the period is 100 samples, the next midpoint is 50 in the future.
                // Ignores period changes until next cycle.
                float half_period = period / 2.0f;
                phase_max = std::floor(0.5f + half_period) - 0.5f;
                dc_offset  = 0.5f * amplitude / phase_max;
                phase_max *= PI;

                phase_inc = phase_max / half_period;
                phase = -phase;

                // Digitial Sine Resonator
                sin0 = amplitude * std::sin(phase);
                sin1 = amplitude * std::sin(phase - phase_inc);
                dsin = 2.0f * std::cos(phase_inc);

                // make sure to handle sin(0)/0. Use a very small number here.
                // Take the square of the phase because the phase can be a very small negative number.
                // In either +- case, we default to the amplitude.

                if (phase * phase > 1e-9) { 
                    output = sin0 / phase;
                } else {
                    output = amplitude;
                }
            } else {
                // Start counting backwards through the rest of the sinc function.
                if (phase > phase_max) {
                    phase = phase_max  + phase_max - phase;
                    phase_inc = -phase_inc;
                }

                // Digitial Sine Resonator
                float sinp = dsin * sin0 - sin1;
                sin1 = sin0;
                sin0 = sinp;

                output = sinp / phase;
            }

            return output - dc_offset;
        }

    private:
        // BLIT
        float phase; // measured in samples * PI
        float phase_max;
        float phase_inc;
        float dc_offset;

        // Digital Sine Resonator
        float sin0;
        float sin1;
        float dsin;

};