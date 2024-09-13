#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_graphics/juce_graphics.h>

class RotaryKnob : public juce::Component {
    public:
        RotaryKnob();
        ~RotaryKnob() override;

        void paint(juce::Graphics&) override;
        void resized() override;
        
        juce::Slider slider;
        juce::String label;

    private:
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RotaryKnob)
};