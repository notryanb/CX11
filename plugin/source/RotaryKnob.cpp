#include "CX11Synth/RotaryKnob.h"

static constexpr int label_height = 15;
static constexpr int text_box_height = 20;

RotaryKnob::RotaryKnob() {
    slider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 100, text_box_height);
    slider.setRotaryParameters(juce::degreesToRadians(225.0f), juce::degreesToRadians(495.0f), true);
    addAndMakeVisible(slider);

    setBounds(0, 0, 100, 120);
}

RotaryKnob::~RotaryKnob() {}

void RotaryKnob::paint(juce::Graphics& g) {
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

    g.setFont(15.0f);
    g.setColour(juce::Colours::white);

    auto bounds = getLocalBounds();
    g.drawText(
        label,
        juce::Rectangle<int> { 0, 0, bounds.getWidth(), label_height },
        juce::Justification::centred
    );
}

void RotaryKnob::resized() {
    auto bounds = getLocalBounds();
    slider.setBounds(0, label_height, bounds.getWidth(), bounds.getHeight() - label_height);
}
