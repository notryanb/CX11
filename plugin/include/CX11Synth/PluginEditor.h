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
    juce::Slider output_level_knob;

    using APVTS = juce::AudioProcessorValueTreeState;
    using SliderAttachment = APVTS::SliderAttachment;

    SliderAttachment output_level_attachment { audioProcessor.apvts, ParameterId::output_level.getParamID(), output_level_knob };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CX11SynthAudioProcessorEditor)
  };

}  // namespace audio_plugin
