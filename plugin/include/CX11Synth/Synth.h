#pragma once

//#include <JuceHeader.h>
#include <stdint.h>
#include "Voice.h"
#include "NoiseGenerator.h"

//namespace audio_plugin {
class Synth {
    public: 
        Synth();

        float noise_mix;

        void allocate_resources(double sample_rate, int /*samples_per_block*/);
        void deallocate_resources();
        void reset();
        void render(float** output_buffers, int sample_count);
        void midi_message(uint8_t data0, uint8_t data1, uint8_t data2);

    private:
        float sample_rate;
        Voice voice;
        NoiseGenerator noise_gen;

        void noteOn(int note, int velocity);
        void noteOff(int note);
};
//} // End NameSpace