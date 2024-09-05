#include "CX11Synth/Synth.h"
#include "CX11Synth/Utils.h"



static const float ANALOG = 0.002f;
static const int SUSTAIN = -1;

//namespace audio_plugin {
Synth::Synth() {
    sample_rate = 44100.0f;
}

void Synth::allocate_resources(double sample_rate_, int /*samples_per_block*/) {
    sample_rate = static_cast<float>(sample_rate_);

    for (int v = 0; v < MAX_VOICES; ++v) {
        voices_[v].filter.sample_rate = sample_rate;
    }
}

void Synth::deallocate_resources() {

}

void Synth::reset() {
        for (int v = 0; v < MAX_VOICES; ++v) {
        voices_[v].reset();
    }

    sustained_pedal_pressed = false;
    pressure = 0.0f;
    lfo_phase = 0.0f;
    lfo_step = 0;
    last_note = 0;
    mod_wheel = 0.0f;
    filter_ctrl= 0.0f;
    filter_zip = 0.0f;

    pitch_bend = 1.0f;
    resonance_ctrl = 1.0f;

    noise_gen.reset();
    output_level_smoother.reset(sample_rate, 0.05);
}

void Synth::render(float** output_buffers, int sample_count) {
    float* output_buffer_left = output_buffers[0];
    float* output_buffer_right = output_buffers[1];

    for (int v = 0; v < MAX_VOICES; ++v) {
        Voice& voice = voices_[v];
        if (voice.env.isActive()) {
            updatePeriod(voice);
            voice.glide_rate = glide_rate;
            voice.filter_q = filter_q * resonance_ctrl;
            voice.pitch_bend = pitch_bend;
        }
    }

    for (int sample = 0; sample < sample_count; ++sample) {
        updateLFO();
        float noise = noise_gen.next_value() * noise_mix;

        float output_left = 0.0f;
        float output_right = 0.0f;

        for (int v = 0; v < MAX_VOICES; ++v) {
            Voice& voice = voices_[v];
            if (voice.env.isActive()) {
                float output = voice.render(noise);
                output_left += output * voice.pan_left;
                output_right += output * voice.pan_right;
            }
        }

        float output_level = output_level_smoother.getNextValue();
        output_left *= output_level;
        output_right *= output_level;

        if (output_buffer_right != nullptr) {
            output_buffer_left[sample] = output_left;
            output_buffer_right[sample] = output_right;
        } else {            
            output_buffer_left[sample] = (output_left + output_right) * 0.5f;;
        }
    }

    for (int v = 0; v < MAX_VOICES; ++v) {
        Voice& voice = voices_[v];
        if (!voice.env.isActive()) {
            voice.env.reset();
            voice.filter.reset();
        }
    }

    protectYourEars(output_buffer_left, sample_count);
    protectYourEars(output_buffer_right, sample_count);
}

void Synth::updateLFO() {
    if (--lfo_step <= 0) {
        lfo_step = LFO_MAX;
        lfo_phase += lfo_inc;

        if (lfo_phase > PI){
            lfo_phase -= TAU;
        }

        const float sine = std::sin(lfo_phase);
        float vibrato_mod = 1.0f + sine * (mod_wheel + vibrato);
        float pwm = 1.0f + sine * (mod_wheel + pwm_depth);
        float filter_mod = filter_key_tracking + filter_ctrl + (filter_lfo_depth + pressure) * sine;
        
        // one-pole filter to make filter mod transitions exponentially smooth
        // 0.005 coefficient is sample-rate dependent
        filter_zip += 0.005f * (filter_mod - filter_zip); 

        for (int v = 0; v < MAX_VOICES; ++v) {
            Voice& voice = voices_[v];
            if (voice.env.isActive()) {
                voice.osc1.modulation = vibrato_mod;
                voice.osc2.modulation = pwm;
                voice.filter_mod = filter_zip;
                voice.update_LFO();
                updatePeriod(voice);
            }
        }
    }
}

void Synth::midi_message(uint8_t data0, uint8_t data1, uint8_t data2) {
    // MSVC is complaining about these not being initialized when I put the pitch bend
    // case at the end. Oh well...
    uint8_t note = 0;
    uint8_t velocity = 0;
    
    switch (data0 & 0xF0) {
        case 0x80:
            noteOff(data1 & 0x7F);
            break;
        case 0x90:
            note = data1 & 0x7F;
            velocity = data2 & 0x7F;

            if (velocity > 0) {
                noteOn(note, velocity);
            } else {
                noteOff(note);
            }
            break;
        case 0xB0:
            controlChange(data1, data2);
            break;
        case 0xD0:
            pressure = 0.0001f * float(data1 * data1); // map 0.0 .. 1.61 (position 127)
            break;
        case 0xE0: // TODO - investigate why the compiler won't let me put this after the noteOn case...?
            pitch_bend = std::exp(-0.000014102f * float(data1 + 128 * data2 - 8192));
            break;
    }
}

void Synth::startVoice(int v, int note, int velocity) {
    float period = calcPeriod(v, note);

    Voice& voice = voices_[v];
    voice.target = period;

    int note_distance = 0;
    if (last_note > 0) {
        if ((glide_mode == 2) || ((glide_mode == 1) && isPlayingLegatoStyle())) {
            note_distance = note - last_note;
        }
    }

    voice.period = period * std::pow(1.059463094359f, float(note_distance) - glide_bend);
    if (voice.period < 6.0f) { voice.period = 6.0f; }

    last_note = note;
    voice.note = note;
    voice.updatePanning();

    float velocity_curve = 0.004f * float((velocity + 64) * (velocity + 64)) - 8.0f;
    voice.osc1.amplitude = volume_trim * velocity_curve;
    voice.osc2.amplitude = voice.osc1.amplitude * osc_mix;

    if (vibrato == 0.0f && pwm_depth > 0.0f) {
        voice.osc2.squareWave(voice.osc1, voice.period);
    }

    voice.cutoff = sample_rate / (period * PI);
    voice.cutoff *= std::exp(velocity_sensitivity * float(velocity - 64));

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

// Finds a voice to use that has the lowest level and is not in attack portion of the envelope.
int Synth::findFreeVoice() const {
    int v = 0;
    float l = 100.0f; // louder than any envelope;

    for (int i = 0; i < MAX_VOICES; ++i) {
        if (voices_[i].env.level < l && !voices_[i].env.isInAttack()) {
            l = voices_[i].env.level;
            v = i;
        }
    }

    return v;
}

void Synth::noteOn(int note, int velocity) {
    if (ignore_velocity) { velocity = 80; }

    int v = 0;

    if (num_voices == 1) {  // monophonic
        if (voices_[0].note > 0) { // legato
            shiftQueuedNotes();
            restartMonoVoice(note, velocity);
            return;
        }
    } else {
        // Polyphonic, num_voices will either be 0 or 8 (MAX_VOICES)
        v = findFreeVoice();
    }

    startVoice(v, note, velocity);
}

void Synth::noteOff(int note) {
    // for Legato playing
    if ((num_voices == 1) && (voices_[0].note == note)) {
        int queuedNote = nextQueuedNote();
        if (queuedNote > 0) {
            restartMonoVoice(queuedNote, -1);
        }
    }

    for (int v = 0; v < MAX_VOICES; v++) {
        if (voices_[v].note == note) {
            if (sustained_pedal_pressed) {
                voices_[v].note = SUSTAIN;
            } else {
                voices_[v].release();
                voices_[v].note = 0;
            }
        }
    }
}

void Synth::controlChange(uint8_t data1, uint8_t data2) {
    switch (data1) {
        case 0x40:
            sustained_pedal_pressed = (data2 >= 64);
            if (!sustained_pedal_pressed) {
                noteOff(SUSTAIN);
            }
            break;
        case 0x01:
            mod_wheel = 0.000005f * float(data2 * data2);
            break;
        case 0x47:
            resonance_ctrl = 154.0f / float (154 - data2);
            break;
        case 0x4A:
            filter_ctrl = 0.02f * float(data2);
            break;
        case 0x4B:
            filter_ctrl = -0.03f * float(data2);
            break;
        default:
            if (data1 >= 0x78) {
                for (int v = 0; v < MAX_VOICES; ++v) {
                    voices_[v].reset();
                }
                sustained_pedal_pressed = false;            
            }
            break;
    }
}

float Synth::calcPeriod(int v, int note) const {
    // Adding the analog constant keeps it ever so slightly out of tune.
    float period = tune * std::exp(-0.05776226505f * (float(note) + ANALOG * float(v)));

    // Ensure the period or detuned period is at least 6 samples long.
    // at 44.1kHz, the highest freq we can produce is 7350Hz (44100 / 6)
    while (period < 6.0f || (period * detune) < 6.0f) {
        period += period;
    }

    return period;
}

void Synth::shiftQueuedNotes() {
    for (int tmp =  MAX_VOICES - 1; tmp > 0; tmp--) {
        voices_[tmp].note = voices_[tmp - 1].note;
        voices_[tmp].release();
    }
}

int Synth::nextQueuedNote() {
    int held = 0;

    for (int v = MAX_VOICES - 1; v > 0; v--) {
        if (voices_[v].note > 0) { held = v; }
    }

    if (held > 0)  {
        int note = voices_[held].note;
        voices_[held].note = 0;
        return note;
    }

    return 0;
}

// In Legato mode, do not restart the envelope or calculate new oscillator amplitudes.
void Synth::restartMonoVoice(int note, int velocity) {
    juce::ignoreUnused(velocity);

    float period = calcPeriod(0, note);

    Voice& voice = voices_[0];
    voice.target = period;

    if (glide_mode == 0) { voice.period = period; }

    voice.cutoff = sample_rate / (period * PI);
    if (velocity > 0) {
        voice.cutoff *= std::exp(velocity_sensitivity * float(velocity - 64));
    }

    voice.env.level += SILENCE + SILENCE;
    voice.note = note;
    voice.updatePanning();
}

bool Synth::isPlayingLegatoStyle() const {
    int held = 0;
    for (int i = 0; i < MAX_VOICES; ++i) {
        if (voices_[i].note > 0) { held += 1; }
    }
    return held > 0;
}
//} // End namespace