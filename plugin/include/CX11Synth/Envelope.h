#pragma once

const float SILENCE = 0.001f; // 20 * log(0.001) = -80dB.

class Envelope {
    public:
        float nextValue() {
            // The same thing as (1 - multiplier) * target + multiplier, which is a one-pole LPF
            // for smoothing out the different parts of the envelope signal. 
            //ie. Moving from decay to sustain to release needs be "gentle" instead of sharp jumps, which can create artifacts.
            level = multiplier * (level - target) + target;
            return level;
        }

        float level;
        float multiplier;
        float target;
};