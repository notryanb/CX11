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

            // Digitial Sine Resonator
            // phase = 1.5707963268f;
            // sin0 = amplitude * std::sin(TAU * phase);
            // sin1 = amplitude * std::sin(TAU * (phase - phase_inc));
            // dsin = 2.0f * std::cos(TAU * phase_inc);
        }

        // float next_band_limited_sample() {
        //     phase_bl += phase_inc;
        //     if (phase_bl >= 1.0f) {
        //         phase_bl -= 1.0f;
        //     }

        //     float output = 0.0f;
        //     float nyquist = sample_rate / 2.0f;
        //     float h = freq;
        //     float i = 1.0f;
        //     float m = 0.6366197724f; // 2 / PI
        //     while (h < nyquist) {
        //         output += m * std::sin(TAU * phase_bl * i) / i;
        //         h += freq;
        //         i += 1.0f;
        //         m = -m;
        //     }

        //     return output;
        // }

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
                phase_max *= PI;

                phase_inc = phase_max / half_period;
                phase = -phase;

                // make sure to handle sin(0)/0. Use a very small number here.
                // Take the square of the phase because the phase can be a very small negative number.
                // In either +- case, we default to the amplitude.
                if (phase * phase > 1e-9) { 
                    output = amplitude * std::sin(phase) / phase;
                } else {
                    output = amplitude;
                }
            } else {
                // Start counting backwards through the rest of the sinc function.
                if (phase > phase_max) {
                    phase = phase_max  + phase_max - phase;
                    phase_inc = -phase_inc;
                }

                output = amplitude * std::sin(phase) / phase;
            }

            return output;


            // Digitial Sine Resonator
            // float sinx = dsin * sin0 - sin1;
            // sin1 = sin0;
            // sin0 = sinx;
            // return sinx;
        }

    private:
        float phase; // measured in samples * PI
        float phase_max;
        float phase_inc;
        // float sin0;
        // float sin1;
        // float dsin;

};