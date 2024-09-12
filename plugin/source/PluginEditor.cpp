#include "CX11Synth/PluginEditor.h"
#include "CX11Synth/PluginProcessor.h"

namespace audio_plugin {

  CX11SynthAudioProcessorEditor::CX11SynthAudioProcessorEditor(CX11SynthAudioProcessor& p) 
    : AudioProcessorEditor(&p), audioProcessor(p) {

    juce::ignoreUnused(audioProcessor);

    output_level_knob.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    output_level_knob.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 100, 20);
    filter_reso_knob.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    filter_reso_knob.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 100, 20);


    addAndMakeVisible(output_level_knob);
    addAndMakeVisible(filter_reso_knob);

    setSize(600, 400);
  }

  CX11SynthAudioProcessorEditor::~CX11SynthAudioProcessorEditor() {}

  void CX11SynthAudioProcessorEditor::paint(juce::Graphics& g) {

    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

    /* Default */
    // g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

    // g.setColour(juce::Colours::white);
    // g.setFont(15.0f);
    // g.drawFittedText("Hello World!", getLocalBounds(), juce::Justification::centred, 1);
  }

  void CX11SynthAudioProcessorEditor::resized() {
    output_level_knob.setBounds(20, 20, 100, 120);
    filter_reso_knob.setBounds(120, 20, 100, 120);
  }

}  // namespace audio_plugin
