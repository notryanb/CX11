#pragma once

#include "PluginProcessor.h"

namespace audio_plugin {

  class CX11SynthAudioProcessorEditor : public juce::AudioProcessorEditor {
  public:
    explicit CX11SynthAudioProcessorEditor(CX11SynthAudioProcessor&);
    ~CX11SynthAudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

  private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    CX11SynthAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CX11SynthAudioProcessorEditor)
  };

}  // namespace audio_plugin
