#include "CX11Synth/PluginEditor.h"
#include "CX11Synth/PluginProcessor.h"

namespace audio_plugin {

  CX11SynthAudioProcessorEditor::CX11SynthAudioProcessorEditor(CX11SynthAudioProcessor& p) 
    : AudioProcessorEditor(&p), audioProcessor(p) {

    juce::ignoreUnused(audioProcessor);
    juce::LookAndFeel::setDefaultLookAndFeel(&globalLNF);

    output_level_knob.label = "Output Level";
    addAndMakeVisible(output_level_knob);

    filter_reso_knob.label = "Filter Reso";
    addAndMakeVisible(filter_reso_knob);

    poly_mode_button.setButtonText("Poly");
    poly_mode_button.setClickingTogglesState(true);
    addAndMakeVisible(poly_mode_button);

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
    juce::Rectangle r(20, 20, 100, 120);
    output_level_knob.setBounds(r);

    r = r.withX(r.getRight() + 20);
    filter_reso_knob.setBounds(r);

    poly_mode_button.setSize(80, 30);
    poly_mode_button.setCentrePosition(r.withX(r.getRight()).getCentre());
  }

}  // namespace audio_plugin
