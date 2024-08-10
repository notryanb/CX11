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
}

void Synth::render(float** output_buffers, int sample_count) {
    float* output_buffer_left = output_buffers[0];
    float* output_buffer_right = output_buffers[1];

    for (int sample = 0; sample < sample_count; ++sample) {
        float noise = noise_gen.next_value();

        float output = 0.0f;

        if (voice.note > 0) {
            output = noise * (voice.velocity / 127.0f) * 0.5f;
        }

        output_buffer_left[sample] = output;

        if (output_buffer_right != nullptr) {
            output_buffer_right[sample] = output;
        }
    }

    protectYourEars(output_buffer_left, sample_count);
    protectYourEars(output_buffer_right, sample_count);
}

void Synth::midi_message(uint8_t data0, uint8_t data1, uint8_t data2) {
    switch (data0 & 0xF0) {
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
    voice.velocity = velocity;
}

void Synth::noteOff(int note) {
    if (voice.note == note) {
        voice.note = 0;
        voice.velocity = 0;
    }
}
//} // End namespace