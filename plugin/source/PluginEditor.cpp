#include "CX11Synth/PluginEditor.h"
#include "CX11Synth/PluginProcessor.h"

#include <iostream>

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

    midi_learn_btn.setButtonText("MIDI Learn");
    midi_learn_btn.addListener(this);
    addAndMakeVisible(midi_learn_btn);

    setSize(600, 400);
  }

  CX11SynthAudioProcessorEditor::~CX11SynthAudioProcessorEditor() {
    midi_learn_btn.removeListener(this);
    audioProcessor.midi_learn = false;
  }

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

    midi_learn_btn.setBounds(400, 20, 100, 30);
  }

  void CX11SynthAudioProcessorEditor::buttonClicked(juce::Button* button) {
    button->setButtonText("Waiting...");
    button->setEnabled(false);
    audioProcessor.midi_learn = true;
    startTimerHz(10);
  }

  void CX11SynthAudioProcessorEditor::timerCallback() {
    if (!audioProcessor.midi_learn) {
      stopTimer();
      midi_learn_btn.setButtonText("MIDI Learn");
      midi_learn_btn.setEnabled(true);
    }
  }

}  // namespace audio_plugin
