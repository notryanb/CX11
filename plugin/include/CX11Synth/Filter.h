#pragma once

#include <cmath>

// state variable filter
class Filter {
    public:
        float sample_rate;

        void updateCoefficients(float cutoff, float q) {
            g = std::tan(PI * cutoff / sample_rate); // cutoff frequency
            k = 1.0f / q; // resonance
            a1 = 1.0f / (1.0f + g * (g + k));
            a2 = g * a1;
            a3 = g * a2;
        }

        void reset() {
            g = 0.0f;
            k = 0.0f;
            a1 = 0.0f;
            a2 = 0.0f;
            a3 = 0.0f;

            ic1eq = 0.0f;
            ic2eq = 0.0f;
        }

        // v1..v3 are voltages at different nodes
        float render(float x) {
            float v3 = x - ic2eq;
            float v1 = a1 * ic1eq + a2 * v3;
            float v2 = ic2eq + a2 * ic1eq + a3 * v3; // lp
            ic1eq = 2.0f * v1 - ic1eq; // state of capacitors
            ic2eq = 2.0f * v2 - ic2eq; // state of capacitors

            return v2;
        }

    private:
        const float PI = 3.1415926535897932f;
        float g, k, a1, a2, a3; // coefficients
        float ic1eq, ic2eq;     // internal state;
};