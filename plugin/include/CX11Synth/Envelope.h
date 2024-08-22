#pragma once

const float SILENCE = 0.001f; // 20 * log(0.001) = -80dB.

class Envelope {
    public:
        float nextValue() {
            // The same thing as (1 - multiplier) * target + multiplier, which is a one-pole LPF
            // for smoothing out the different parts of the envelope signal. 
            //ie. Moving from decay to sustain to release needs be "gentle" instead of sharp jumps, which can create artifacts.
            level = multiplier * (level - target) + target;

            if (level + target > 3.0f) {
                multiplier = decay_multiplier;
                target = sustain_level;
            }

            return level;
        }

        void reset() {
            level = 0.0f;
            target = 0.0f;
            multiplier = 0.0f;
        }

        void release() {
            target = 0.0f;
            multiplier = release_multiplier;
        }

        void attack() {
            level += SILENCE + SILENCE; //  give initial boost so the initial envelope is always greater than SILENCE.
            target = 2.0f;
            multiplier = attack_multiplier;
        }

        inline bool isInAttack() const {
            return target >= 2.0f;
        }

        inline bool isActive() const {
            return level > SILENCE;
        }

        float level;
        float attack_multiplier;
        float decay_multiplier;
        float sustain_level;
        float release_multiplier;
        
    private:
        float multiplier;
        float target;
};