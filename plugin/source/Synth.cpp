#include "CX11Synth/Synth.h"
#include "CX11Synth/Utils.h"

//namespace audio_plugin {
Synth::Synth() {
    sample_rate = 44100.0f;
}
void Synth::allocate_resources(double sample_rate_, int /*samples_per_block*/) {
    sample_rate = static_cast<float>(sample_rate_);
}

void Synth::deallocate_resources() {

}

void Synth::reset() {
    voice.reset();
    noise_gen.reset();
    pitch_bend = 1.0f;
}

void Synth::render(float** output_buffers, int sample_count) {
    float* output_buffer_left = output_buffers[0];
    float* output_buffer_right = output_buffers[1];

    voice.osc1.period = voice.period * pitch_bend;
    voice.osc2.period = voice.osc1.period * detune;

    for (int sample = 0; sample < sample_count; ++sample) {
        float noise = noise_gen.next_value() * noise_mix;

        float output_left = 0.0f;
        float output_right = 0.0f;

        if (voice.env.isActive()) {
            float output = voice.render(noise);
            output_left = output * voice.pan_left;
            output_right = output * voice.pan_right;
        }

        if (output_buffer_right != nullptr) {
            output_buffer_left[sample] = output_left;
            output_buffer_right[sample] = output_right;
        } else {
            
        output_buffer_left[sample] = (output_left + output_right) * 0.5f;;
        }
    }

    if (!voice.env.isActive()) {
        voice.env.reset();
    }

    protectYourEars(output_buffer_left, sample_count);
    protectYourEars(output_buffer_right, sample_count);
}

void Synth::midi_message(uint8_t data0, uint8_t data1, uint8_t data2) {
    switch (data0 & 0xF0) {
        case 0xE0: // TODO - investigate why the compiler won't let me put this after the noteOn case...?
            pitch_bend = std::exp(-0.000014102f * float(data1 + 128 * data2 - 8192));
            break;
        case 0x80:
            noteOff(data1 & 0x7F);
            break;
        case 0x90:
            uint8_t note = data1 & 0x7F;
            uint8_t velocity = data2 & 0x7F;

            if (velocity > 0) {
                noteOn(note, velocity);
            } else {
                noteOff(note);
            }
            break;
    }
}

void Synth::noteOn(int note, int velocity) {
    voice.note = note;
    voice.updatePanning();

    // Converts midi note to frequency
    float period = calcPeriod(note);
    voice.period = period;
    voice.osc1.amplitude = (velocity / 127.0f) * 0.5f;
    voice.osc2.amplitude = voice.osc1.amplitude * osc_mix;

    // OPTIONAL: Resetting the phase on notes between the oscillators changes the way the notes sound
    // when you play the same thing repeatedly. See which one sounds better.
    // voice.osc1.reset();
    // voice.osc2.reset();

    Envelope& env = voice.env;
    env.attack_multiplier = env_attack;
    env.decay_multiplier = env_decay;
    env.sustain_level = env_sustain;
    env.release_multiplier = env_release;
    env.attack();
}

void Synth::noteOff(int note) {
    if (voice.note == note) {
        voice.release();
    }
}

float Synth::calcPeriod(int note) const {
    float period = tune * std::exp(-0.05776226505f * float(note));

    // Ensure the period or detuned period is at least 6 samples long.
    // at 44.1kHz, the highest freq we can produce is 7350Hz (44100 / 6)
    while (period < 6.0f || (period * detune) < 6.0f) {
        period += period;
    }

    return period;
}
//} // End namespace