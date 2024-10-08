#pragma once

#include <algorithm>
#include "Envelope.h"
#include "Filter.h"
#include "Oscillator.h"

struct Voice {
    int note;
    float saw;
    float period;
    float pan_left, pan_right;
    float target;
    float glide_rate;
    float cutoff;
    float filter_mod;
    float filter_q;
    float pitch_bend; // multiplier based on number of semitones
    float filter_env_depth;

    Envelope env;
    Envelope filter_env;
    Filter filter;
    Oscillator osc1;
    Oscillator osc2;

    void reset() {
        note = 0;
        saw = 0.0f;
        pan_left = 0.707f;
        pan_right = 0.707f;
        filter.reset();
        osc1.reset();
        osc2.reset();
        env.reset();
        filter_env.reset();
    }

    void release() {
        env.release();
        filter_env.release();
    }

    float render(float input) {
        // 0.997f is a "leaky" integrator. Acts as a LPF that prevents an offset from building up.
        float sample1 = osc1.next_sample();
        float sample2 = osc2.next_sample();

        saw = saw * 0.997f + sample1 - sample2;
         
        /*
            Ordering of the filter vs envelope application is irrelevant if the system is
            LTI (linear time-invariant).
        
        */
        float output = saw + input;       // mixes in the noise
        output = filter.render(output);   // apply the filter
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

    void update_LFO() {
        period += glide_rate * (target - period);
        float fenv = filter_env.nextValue();
        float modulated_cutoff =  cutoff * std::exp(filter_mod + filter_env_depth * fenv) / pitch_bend;
        modulated_cutoff = std::clamp(modulated_cutoff, 30.0f, 20000.0f);
        filter.updateCoefficients(modulated_cutoff, filter_q); // 0.707 (sqrt(.5)) means no resonance. Consider this the minimum value
    }
};