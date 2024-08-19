#pragma once


#include <cmath>
#include <juce_audio_processors/juce_audio_processors.h> // This is how I'm importing DBG... seems wrong?

inline void protectYourEars(float* buffer, int sample_count)
{
    if (buffer == nullptr) { return; }
    bool first_warning = true;
    for (int i = 0; i < sample_count; ++i) {
        float x = buffer[i];
        bool silence = false;
        if (std::isnan(x)) {
            DBG("!!! WARNING: nan detected in audio buffer, silencing !!!");
            silence = true;
        } else if (std::isinf(x)) {
            DBG("!!! WARNING: inf detected in audio buffer, silencing !!!");
            silence = true;
        } else if (x < -2.0f || x > 2.0f) {  // screaming feedback
            DBG("!!! WARNING: sample out of range, silencing !!!");
            silence = true;
        } else if (x < -1.0f) {
            if (first_warning) {
                DBG("!!! WARNING: sample out of range, clamping !!!");
                first_warning = false;
            }
            buffer[i] = -1.0f;
        } else if (x > 1.0f) {
            if (first_warning) {
                DBG("!!! WARNING: sample out of range, clamping !!!");
                first_warning = false;
            }
            buffer[i] = 1.0f;
        }
        if (silence) {
            memset(buffer, 0, sample_count * sizeof(float));
            return;
        }
    }
}

template<typename T>
inline static void castParameter(
    juce::AudioProcessorValueTreeState& apvts, 
    const juce::ParameterID& id, 
    T& destination) {
    destination = dynamic_cast<T>(apvts.getParameter(id.getParamID()));
    jassert(destination);
}