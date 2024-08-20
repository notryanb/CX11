#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include "Synth.h"
#include "Preset.h"

namespace ParameterId {
  #define PARAMETER_ID(str) const juce::ParameterID str(#str, 1);

  PARAMETER_ID(osc_mix);
  PARAMETER_ID(osc_tune);
  PARAMETER_ID(osc_fine);
  PARAMETER_ID(glide_mode);
  PARAMETER_ID(glide_rate);
  PARAMETER_ID(glide_bend);
  PARAMETER_ID(filter_freq);
  PARAMETER_ID(filter_reso);
  PARAMETER_ID(filter_env);
  PARAMETER_ID(filter_lfo);
  PARAMETER_ID(filter_velocity);
  PARAMETER_ID(filter_attack);
  PARAMETER_ID(filter_decay);
  PARAMETER_ID(filter_sustain);
  PARAMETER_ID(filter_release);
  PARAMETER_ID(env_attack);
  PARAMETER_ID(env_decay);
  PARAMETER_ID(env_sustain);
  PARAMETER_ID(env_release);
  PARAMETER_ID(lfo_rate);
  PARAMETER_ID(vibrato);
  PARAMETER_ID(noise);
  PARAMETER_ID(octave);
  PARAMETER_ID(tuning);
  PARAMETER_ID(output_level);
  PARAMETER_ID(poly_mode);

  #undef PARAMETER_ID
}

namespace audio_plugin {
  class CX11SynthAudioProcessor : public juce::AudioProcessor,
                                  private juce::ValueTree::Listener {
  public:
      juce::AudioProcessorValueTreeState apvts { *this, nullptr, "Parameters", createParameterLayout() };

      CX11SynthAudioProcessor();
      ~CX11SynthAudioProcessor() override;

      void prepareToPlay(double sampleRate, int samplesPerBlock) override;
      void releaseResources() override;
      void reset() override;

      bool isBusesLayoutSupported(const BusesLayout& layouts) const override;

      void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
      using AudioProcessor::processBlock;

      juce::AudioProcessorEditor* createEditor() override;
      bool hasEditor() const override;

      const juce::String getName() const override;

      bool acceptsMidi() const override;
      bool producesMidi() const override;
      bool isMidiEffect() const override;
      double getTailLengthSeconds() const override;

      int getNumPrograms() override;
      int getCurrentProgram() override;
      void setCurrentProgram(int index) override;
      const juce::String getProgramName(int index) override;
      void changeProgramName(int index, const juce::String& newName) override;

      void getStateInformation(juce::MemoryBlock& destData) override;
      void setStateInformation(const void* data, int sizeInBytes) override;
  private:
      Synth synth;

      std::atomic<bool> parametersChanged { false };
      std::vector<Preset> presets;
      int currentProgram;

      juce::AudioParameterFloat* osc_mix_param;
      juce::AudioParameterFloat* osc_tune_param;
      juce::AudioParameterFloat* osc_fine_param;
      juce::AudioParameterChoice* glide_mode_param;
      juce::AudioParameterFloat* glide_rate_param;
      juce::AudioParameterFloat* glide_bend_param;
      juce::AudioParameterFloat* filter_freq_param;
      juce::AudioParameterFloat* filter_reso_param;
      juce::AudioParameterFloat* filter_env_param;
      juce::AudioParameterFloat* filter_lfo_param;
      juce::AudioParameterFloat* filter_velocity_param;
      juce::AudioParameterFloat* filter_attack_param;
      juce::AudioParameterFloat* filter_decay_param;
      juce::AudioParameterFloat* filter_sustain_param;
      juce::AudioParameterFloat* filter_release_param;
      juce::AudioParameterFloat* env_attack_param;
      juce::AudioParameterFloat* env_decay_param;
      juce::AudioParameterFloat* env_sustain_param;
      juce::AudioParameterFloat* env_release_param;
      juce::AudioParameterFloat* lfo_rate_param;
      juce::AudioParameterFloat* vibrato_param;
      juce::AudioParameterFloat* noise_param;
      juce::AudioParameterFloat* octave_param;
      juce::AudioParameterFloat* tuning_param;
      juce::AudioParameterFloat* output_level_param;
      juce::AudioParameterChoice* poly_mode_param;

      juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
      void createPrograms();
      void update();
      void splitBufferByEvents(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages);
      void handleMIDI(uint8_t data0, uint8_t data1, uint8_t data2);
      void render(juce::AudioBuffer<float>& buffer, int sampleCount, int bufferOffset);

      void valueTreePropertyChanged(juce::ValueTree&, const juce::Identifier&) override {
        parametersChanged.store(true);
      }

      JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CX11SynthAudioProcessor)
  };
}  // namespace audio_plugin
