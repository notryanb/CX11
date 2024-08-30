#pragma once

//#include <JuceHeader.h>
#include <stdint.h>
#include <array>
#include "Voice.h"
#include "NoiseGenerator.h"

#include <juce_audio_processors/juce_audio_processors.h> 

//namespace audio_plugin {
class Synth {
    public: 
        Synth();

        static constexpr int MAX_VOICES = 8;
        const int LFO_MAX = 32;

        int num_voices;
        float lfo_inc;
        float noise_mix;
        float env_attack;
        float env_decay;
        float env_sustain;
        float env_release;
        float osc_mix;
        float detune;
        float tune;
        float pitch_bend;
        float volume_trim;
        float velocity_sensitivity;
        float vibrato;
        float pwm_depth;
        float mod_wheel;
        int glide_mode;
        float glide_rate;
        float glide_bend;
        int last_note;
        float filter_key_tracking;
        float filter_q;
        float resonance_ctrl;
        bool ignore_velocity;

        juce::LinearSmoothedValue<float> output_level_smoother;

        void allocate_resources(double sample_rate, int /*samples_per_block*/);
        void deallocate_resources();
        void reset();
        void render(float** output_buffers, int sample_count);
        void midi_message(uint8_t data0, uint8_t data1, uint8_t data2);
        void controlChange(uint8_t data1, uint8_t data2);

    private:
        float sample_rate;
        float lfo_phase;
        int lfo_step;
        bool sustained_pedal_pressed;

        std::array<Voice, MAX_VOICES> voices_;
        NoiseGenerator noise_gen;

        void updateLFO();
        void shiftQueuedNotes();
        int nextQueuedNote();
        void restartMonoVoice(int note, int velocity);
        int findFreeVoice() const;
        void startVoice(int v, int note, int velocity);
        void noteOn(int note, int velocity);
        void noteOff(int note);
        float calcPeriod(int v, int note) const;
        bool isPlayingLegatoStyle() const;

        inline void updatePeriod(Voice& voice) {
            voice.osc1.period = voice.period * pitch_bend;
            voice.osc2.period = voice.osc1.period * detune;
        }
};
//} // End NameSpace