#include "CX11Synth/PluginEditor.h"
#include "CX11Synth/PluginProcessor.h"

namespace audio_plugin {

  CX11SynthAudioProcessorEditor::CX11SynthAudioProcessorEditor(CX11SynthAudioProcessor& p) 
    : AudioProcessorEditor(&p), audioProcessor(p) {

    juce::ignoreUnused(audioProcessor);

    setSize(400, 300);
  }

  CX11SynthAudioProcessorEditor::~CX11SynthAudioProcessorEditor() {}

  void CX11SynthAudioProcessorEditor::paint(juce::Graphics& g) {
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

    g.setColour(juce::Colours::white);
    g.setFont(15.0f);
    g.drawFittedText("Hello World!", getLocalBounds(), juce::Justification::centred, 1);
  }

  void CX11SynthAudioProcessorEditor::resized() {}

}  // namespace audio_plugin
