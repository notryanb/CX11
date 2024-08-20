#include "CX11Synth/PluginProcessor.h"
#include "CX11Synth/PluginEditor.h"
#include "CX11Synth/Utils.h"

namespace audio_plugin {
CX11SynthAudioProcessor::CX11SynthAudioProcessor()
    : AudioProcessor(
          BusesProperties()
#if !JucePlugin_IsMidiEffect
#if !JucePlugin_IsSynth
              .withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
              .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
) {
  castParameter(apvts, ParameterId::osc_mix, osc_mix_param);
  castParameter(apvts, ParameterId::osc_tune, osc_tune_param);
  castParameter(apvts, ParameterId::osc_fine, osc_fine_param);
  castParameter(apvts, ParameterId::glide_mode, glide_mode_param);
  castParameter(apvts, ParameterId::glide_rate, glide_rate_param);
  castParameter(apvts, ParameterId::glide_bend, glide_bend_param);
  castParameter(apvts, ParameterId::filter_freq, filter_freq_param);
  castParameter(apvts, ParameterId::filter_reso, filter_reso_param);
  castParameter(apvts, ParameterId::filter_env, filter_env_param);
  castParameter(apvts, ParameterId::filter_lfo, filter_lfo_param);
  castParameter(apvts, ParameterId::filter_velocity, filter_velocity_param);
  castParameter(apvts, ParameterId::filter_attack, filter_attack_param);
  castParameter(apvts, ParameterId::filter_decay, filter_decay_param);
  castParameter(apvts, ParameterId::filter_sustain, filter_sustain_param);
  castParameter(apvts, ParameterId::filter_release, filter_release_param);
  castParameter(apvts, ParameterId::env_attack, env_attack_param);
  castParameter(apvts, ParameterId::env_decay, env_decay_param);
  castParameter(apvts, ParameterId::env_sustain, env_sustain_param);
  castParameter(apvts, ParameterId::env_release, env_release_param);
  castParameter(apvts, ParameterId::lfo_rate, lfo_rate_param);
  castParameter(apvts, ParameterId::vibrato, vibrato_param);
  castParameter(apvts, ParameterId::noise, noise_param);
  castParameter(apvts, ParameterId::octave, octave_param);
  castParameter(apvts, ParameterId::tuning, tuning_param);
  castParameter(apvts, ParameterId::output_level, output_level_param);
  castParameter(apvts, ParameterId::poly_mode, poly_mode_param);

  apvts.state.addListener(this);
  createPrograms();
  setCurrentProgram(0);
}

CX11SynthAudioProcessor::~CX11SynthAudioProcessor() {
  apvts.state.removeListener(this);
}

const juce::String CX11SynthAudioProcessor::getName() const {
  return JucePlugin_Name;
}

bool CX11SynthAudioProcessor::acceptsMidi() const {
#if JucePlugin_WantsMidiInput
  return true;
#else
  return false;
#endif
}

bool CX11SynthAudioProcessor::producesMidi() const {
#if JucePlugin_ProducesMidiOutput
  return true;
#else
  return false;
#endif
}

bool CX11SynthAudioProcessor::isMidiEffect() const {
#if JucePlugin_IsMidiEffect
  return true;
#else
  return false;
#endif
}

double CX11SynthAudioProcessor::getTailLengthSeconds() const {
  return 0.0;
}

int CX11SynthAudioProcessor::getNumPrograms() {
  // return 1;  // NB: some hosts don't cope very well if you tell them there are 0
  //            // programs, so this should be at least 1, even if you're not
  //            // really implementing programs.
  return int(presets.size());
}

int CX11SynthAudioProcessor::getCurrentProgram() {
  return currentProgram;
}

void CX11SynthAudioProcessor::setCurrentProgram(int index) {
  currentProgram = index;
  juce::RangedAudioParameter *params[NUM_PARAMS] = {
    osc_mix_param,
    osc_tune_param,
    osc_fine_param,
    glide_mode_param,
    glide_rate_param,
    glide_bend_param,
    filter_freq_param,
    filter_reso_param,
    filter_env_param,
    filter_lfo_param,
    filter_velocity_param,
    filter_attack_param,
    filter_decay_param,
    filter_sustain_param,
    filter_release_param,
    env_attack_param,
    env_decay_param,
    env_sustain_param,
    env_release_param,
    lfo_rate_param,
    vibrato_param,
    noise_param,
    octave_param,
    tuning_param,
    output_level_param,
    poly_mode_param,
  };

  const Preset& preset = presets[index];

  for (int i = 0; i < NUM_PARAMS; ++i) {
    // JUCE uses values 0.0..1.0 for all parameters, so convert the parameters to those values.
    params[i]->setValueNotifyingHost(params[i]->convertTo0to1(preset.param[i]));
  }

  // Tell synth to stop playing. If any notes are playing, sudden changes in the params
  // can cause invalid states in features like filters which can cause poles to be nan/infinity.
  reset();
}

void CX11SynthAudioProcessor::createPrograms()
{
    presets.emplace_back("Init", 0.00f, -12.00f, 0.00f, 0.00f, 35.00f, 0.00f, 100.00f, 15.00f, 50.00f, 0.00f, 0.00f, 0.00f, 30.00f, 0.00f, 25.00f, 0.00f, 50.00f, 100.00f, 30.00f, 0.81f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 1.00f);
    presets.emplace_back("5th Sweep Pad", 100.00f, -7.00f, -6.30f, 1.00f, 32.00f, 0.00f, 90.00f, 60.00f, -76.00f, 0.00f, 0.00f, 90.00f, 89.00f, 90.00f, 73.00f, 0.00f, 50.00f, 100.00f, 71.00f, 0.81f, 30.00f, 0.00f, 0.00f, 0.00f, 0.00f, 1.00f);
    presets.emplace_back("Echo Pad [SA]", 88.00f, 0.00f, 0.00f, 0.00f, 49.00f, 0.00f, 46.00f, 76.00f, 38.00f, 10.00f, 38.00f, 100.00f, 86.00f, 76.00f, 57.00f, 30.00f, 80.00f, 68.00f, 66.00f, 0.79f, -74.00f, 25.00f, 0.00f, 0.00f, 0.00f, 1.00f);
    presets.emplace_back("Space Chimes [SA]", 88.00f, 0.00f, 0.00f, 0.00f, 49.00f, 0.00f, 49.00f, 82.00f, 32.00f, 8.00f, 78.00f, 85.00f, 69.00f, 76.00f, 47.00f, 12.00f, 22.00f, 55.00f, 66.00f, 0.89f, -32.00f, 0.00f, 2.00f, 0.00f, 0.00f, 1.00f);
    presets.emplace_back("Solid Backing", 100.00f, -12.00f, -18.70f, 0.00f, 35.00f, 0.00f, 30.00f, 25.00f, 40.00f, 0.00f, 26.00f, 0.00f, 35.00f, 0.00f, 25.00f, 0.00f, 50.00f, 100.00f, 30.00f, 0.81f, 0.00f, 50.00f, 0.00f, 0.00f, 0.00f, 1.00f);
    presets.emplace_back("Velocity Backing [SA]", 41.00f, 0.00f, 9.70f, 0.00f, 8.00f, -1.68f, 49.00f, 1.00f, -32.00f, 0.00f, 86.00f, 61.00f, 87.00f, 100.00f, 93.00f, 11.00f, 48.00f, 98.00f, 32.00f, 0.81f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 1.00f);
    presets.emplace_back("Rubber Backing [ZF]", 29.00f, 12.00f, -5.60f, 0.00f, 18.00f, 5.06f, 35.00f, 15.00f, 54.00f, 14.00f, 8.00f, 0.00f, 42.00f, 13.00f, 21.00f, 0.00f, 56.00f, 0.00f, 32.00f, 0.20f, 16.00f, 22.00f, 0.00f, 0.00f, 0.00f, 1.00f);
    presets.emplace_back("808 State Lead", 100.00f, 7.00f, -7.10f, 2.00f, 34.00f, 12.35f, 65.00f, 63.00f, 50.00f, 16.00f, 0.00f, 0.00f, 30.00f, 0.00f, 25.00f, 17.00f, 50.00f, 100.00f, 3.00f, 0.81f, 0.00f, 0.00f, 1.00f, 0.00f, 0.00f, 1.00f);
    presets.emplace_back("Mono Glide", 0.00f, -12.00f, 0.00f, 2.00f, 46.00f, 0.00f, 51.00f, 0.00f, 0.00f, 0.00f, -100.00f, 0.00f, 30.00f, 0.00f, 25.00f, 37.00f, 50.00f, 100.00f, 38.00f, 0.81f, 24.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f);
    presets.emplace_back("Detuned Techno Lead", 84.00f, 0.00f, -17.20f, 2.00f, 41.00f, -0.15f, 54.00f, 1.00f, 16.00f, 21.00f, 34.00f, 0.00f, 9.00f, 100.00f, 25.00f, 20.00f, 85.00f, 100.00f, 30.00f, 0.83f, -82.00f, 40.00f, 0.00f, 0.00f, 0.00f, 1.00f);
    presets.emplace_back("Hard Lead [SA]", 71.00f, 12.00f, 0.00f, 0.00f, 24.00f, 36.00f, 56.00f, 52.00f, 38.00f, 19.00f, 40.00f, 100.00f, 14.00f, 65.00f, 95.00f, 7.00f, 91.00f, 100.00f, 15.00f, 0.84f, -34.00f, 0.00f, 0.00f, 0.00f, 0.00f, 1.00f);
    presets.emplace_back("Bubble", 0.00f, -12.00f, -0.20f, 0.00f, 71.00f, -0.00f, 23.00f, 77.00f, 60.00f, 32.00f, 26.00f, 40.00f, 18.00f, 66.00f, 14.00f, 0.00f, 38.00f, 65.00f, 16.00f, 0.48f, 0.00f, 0.00f, 1.00f, 0.00f, 0.00f, 1.00f);
    presets.emplace_back("Monosynth", 62.00f, -12.00f, 0.00f, 1.00f, 35.00f, 0.02f, 64.00f, 39.00f, 2.00f, 65.00f, -100.00f, 7.00f, 52.00f, 24.00f, 84.00f, 13.00f, 30.00f, 76.00f, 21.00f, 0.58f, -40.00f, 0.00f, -1.00f, 0.00f, 0.00f, 0.00f);
    presets.emplace_back("Moogcury Lite", 81.00f, 24.00f, -9.80f, 1.00f, 15.00f, -0.97f, 39.00f, 17.00f, 38.00f, 40.00f, 24.00f, 0.00f, 47.00f, 19.00f, 37.00f, 0.00f, 50.00f, 20.00f, 33.00f, 0.38f, 6.00f, 0.00f, -2.00f, 0.00f, 0.00f, 0.00f);
    presets.emplace_back("Gangsta Whine", 0.00f, 0.00f, 0.00f, 2.00f, 44.00f, 0.00f, 41.00f, 46.00f, 0.00f, 0.00f, -100.00f, 0.00f, 0.00f, 100.00f, 25.00f, 15.00f, 50.00f, 100.00f, 32.00f, 0.81f, -2.00f, 0.00f, 2.00f, 0.00f, 0.00f, 0.00f);
    presets.emplace_back("Higher Synth [ZF]", 48.00f, 0.00f, -8.80f, 0.00f, 0.00f, 0.00f, 50.00f, 47.00f, 46.00f, 30.00f, 60.00f, 0.00f, 10.00f, 0.00f, 7.00f, 0.00f, 42.00f, 0.00f, 22.00f, 0.21f, 18.00f, 16.00f, 2.00f, 0.00f, 0.00f, 1.00f);
    presets.emplace_back("303 Saw Bass", 0.00f, 0.00f, 0.00f, 1.00f, 49.00f, 0.00f, 55.00f, 75.00f, 38.00f, 35.00f, 0.00f, 0.00f, 56.00f, 0.00f, 56.00f, 0.00f, 80.00f, 100.00f, 24.00f, 0.26f, -2.00f, 0.00f, -2.00f, 0.00f, 0.00f, 0.00f);
    presets.emplace_back("303 Square Bass", 75.00f, 0.00f, 0.00f, 1.00f, 49.00f, 0.00f, 55.00f, 75.00f, 38.00f, 35.00f, 0.00f, 14.00f, 49.00f, 0.00f, 39.00f, 0.00f, 80.00f, 100.00f, 24.00f, 0.26f, -2.00f, 0.00f, -2.00f, 0.00f, 0.00f, 0.00f);
    presets.emplace_back("Analog Bass", 100.00f, -12.00f, -10.90f, 1.00f, 19.00f, 0.00f, 30.00f, 51.00f, 70.00f, 9.00f, -100.00f, 0.00f, 88.00f, 0.00f, 21.00f, 0.00f, 50.00f, 100.00f, 46.00f, 0.81f, 0.00f, 0.00f, -1.00f, 0.00f, 0.00f, 0.00f);
    presets.emplace_back("Analog Bass 2", 100.00f, -12.00f, -10.90f, 0.00f, 19.00f, 13.44f, 48.00f, 43.00f, 88.00f, 0.00f, 60.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 61.00f, 100.00f, 32.00f, 0.81f, 0.00f, 0.00f, -1.00f, 0.00f, 0.00f, 0.00f);
    presets.emplace_back("Low Pulses", 97.00f, -12.00f, -3.30f, 0.00f, 35.00f, 0.00f, 80.00f, 40.00f, 4.00f, 0.00f, 0.00f, 0.00f, 77.00f, 0.00f, 25.00f, 0.00f, 50.00f, 100.00f, 30.00f, 0.81f, -68.00f, 0.00f, -2.00f, 0.00f, 0.00f, 1.00f);
    presets.emplace_back("Sine Infra-Bass", 0.00f, -12.00f, 0.00f, 0.00f, 35.00f, 0.00f, 33.00f, 76.00f, 6.00f, 0.00f, 0.00f, 0.00f, 30.00f, 0.00f, 25.00f, 0.00f, 55.00f, 25.00f, 30.00f, 0.81f, 4.00f, 0.00f, -2.00f, 0.00f, 0.00f, 0.00f);
    presets.emplace_back("Wobble Bass [SA]", 100.00f, -12.00f, -8.80f, 0.00f, 82.00f, 0.21f, 72.00f, 47.00f, -32.00f, 34.00f, 64.00f, 20.00f, 69.00f, 100.00f, 15.00f, 9.00f, 50.00f, 100.00f, 7.00f, 0.81f, -8.00f, 0.00f, -1.00f, 0.00f, 0.00f, 0.00f);
    presets.emplace_back("Squelch Bass", 100.00f, -12.00f, -8.80f, 0.00f, 35.00f, 0.00f, 67.00f, 70.00f, -48.00f, 0.00f, 0.00f, 48.00f, 69.00f, 100.00f, 15.00f, 0.00f, 50.00f, 100.00f, 7.00f, 0.81f, -8.00f, 0.00f, -1.00f, 0.00f, 0.00f, 0.00f);
    presets.emplace_back("Rubber Bass [ZF]", 49.00f, -12.00f, 1.60f, 1.00f, 35.00f, 0.00f, 36.00f, 15.00f, 50.00f, 20.00f, 0.00f, 0.00f, 38.00f, 0.00f, 25.00f, 0.00f, 60.00f, 100.00f, 22.00f, 0.19f, 0.00f, 0.00f, -2.00f, 0.00f, 0.00f, 0.00f);
    presets.emplace_back("Soft Pick Bass", 37.00f, 0.00f, 7.80f, 0.00f, 22.00f, 0.00f, 33.00f, 47.00f, 42.00f, 16.00f, 18.00f, 0.00f, 0.00f, 0.00f, 25.00f, 4.00f, 58.00f, 0.00f, 22.00f, 0.15f, -12.00f, 33.00f, -2.00f, 0.00f, 0.00f, 0.00f);
    presets.emplace_back("Fretless Bass", 50.00f, 0.00f, -14.40f, 1.00f, 34.00f, 0.00f, 51.00f, 0.00f, 16.00f, 0.00f, 34.00f, 0.00f, 9.00f, 0.00f, 25.00f, 20.00f, 85.00f, 0.00f, 30.00f, 0.81f, 40.00f, 0.00f, -2.00f, 0.00f, 0.00f, 0.00f);
    presets.emplace_back("Whistler", 23.00f, 0.00f, -0.70f, 0.00f, 35.00f, 0.00f, 33.00f, 100.00f, 0.00f, 0.00f, 0.00f, 0.00f, 29.00f, 0.00f, 25.00f, 68.00f, 39.00f, 58.00f, 36.00f, 0.81f, 28.00f, 38.00f, 2.00f, 0.00f, 0.00f, 1.00f);
    presets.emplace_back("Very Soft Pad", 39.00f, 0.00f, -4.90f, 2.00f, 12.00f, 0.00f, 35.00f, 78.00f, 0.00f, 0.00f, 0.00f, 0.00f, 30.00f, 0.00f, 25.00f, 35.00f, 50.00f, 80.00f, 70.00f, 0.81f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 1.00f);
    presets.emplace_back("Pizzicato", 0.00f, -12.00f, 0.00f, 0.00f, 35.00f, 0.00f, 23.00f, 20.00f, 50.00f, 0.00f, 0.00f, 0.00f, 22.00f, 0.00f, 25.00f, 0.00f, 47.00f, 0.00f, 30.00f, 0.81f, 0.00f, 80.00f, 0.00f, 0.00f, 0.00f, 1.00f);
    presets.emplace_back("Synth Strings", 100.00f, 0.00f, -7.10f, 0.00f, 0.00f, -0.97f, 42.00f, 26.00f, 50.00f, 14.00f, 38.00f, 0.00f, 67.00f, 55.00f, 97.00f, 82.00f, 70.00f, 100.00f, 42.00f, 0.84f, 34.00f, 30.00f, 0.00f, 0.00f, 0.00f, 1.00f);
    presets.emplace_back("Synth Strings 2", 75.00f, 0.00f, -3.80f, 0.00f, 49.00f, 0.00f, 55.00f, 16.00f, 38.00f, 8.00f, -60.00f, 76.00f, 29.00f, 76.00f, 100.00f, 46.00f, 80.00f, 100.00f, 39.00f, 0.79f, -46.00f, 0.00f, 1.00f, 0.00f, 0.00f, 1.00f);
    presets.emplace_back("Leslie Organ", 0.00f, 0.00f, 0.00f, 0.00f, 13.00f, -0.38f, 38.00f, 74.00f, 8.00f, 20.00f, -100.00f, 0.00f, 55.00f, 52.00f, 31.00f, 0.00f, 17.00f, 73.00f, 28.00f, 0.87f, -52.00f, 0.00f, -1.00f, 0.00f, 0.00f, 1.00f);
    presets.emplace_back("Click Organ", 50.00f, 12.00f, 0.00f, 0.00f, 35.00f, 0.00f, 44.00f, 50.00f, 30.00f, 16.00f, -100.00f, 0.00f, 0.00f, 18.00f, 0.00f, 0.00f, 75.00f, 80.00f, 0.00f, 0.81f, -2.00f, 0.00f, 0.00f, 0.00f, 0.00f, 1.00f);
    presets.emplace_back("Hard Organ", 89.00f, 19.00f, -0.90f, 0.00f, 35.00f, 0.00f, 51.00f, 62.00f, 8.00f, 0.00f, -100.00f, 0.00f, 37.00f, 0.00f, 100.00f, 4.00f, 8.00f, 72.00f, 4.00f, 0.77f, -2.00f, 0.00f, 0.00f, 0.00f, 0.00f, 1.00f);
    presets.emplace_back("Bass Clarinet", 100.00f, 0.00f, 0.00f, 1.00f, 0.00f, 0.00f, 51.00f, 10.00f, 0.00f, 11.00f, 0.00f, 0.00f, 0.00f, 0.00f, 25.00f, 35.00f, 65.00f, 65.00f, 32.00f, 0.79f, -2.00f, 20.00f, -1.00f, 0.00f, 0.00f, 1.00f);
    presets.emplace_back("Trumpet", 0.00f, 0.00f, 0.00f, 1.00f, 6.00f, 0.00f, 57.00f, 0.00f, -36.00f, 15.00f, 0.00f, 21.00f, 15.00f, 0.00f, 25.00f, 24.00f, 60.00f, 80.00f, 10.00f, 0.75f, 10.00f, 25.00f, 1.00f, 0.00f, 0.00f, 0.00f);
    presets.emplace_back("Soft Horn", 12.00f, 19.00f, 1.90f, 0.00f, 35.00f, 0.00f, 50.00f, 21.00f, -42.00f, 12.00f, 20.00f, 0.00f, 35.00f, 36.00f, 25.00f, 8.00f, 50.00f, 100.00f, 27.00f, 0.83f, 2.00f, 10.00f, -1.00f, 0.00f, 0.00f, 1.00f);
    presets.emplace_back("Brass Section", 43.00f, 12.00f, -7.90f, 0.00f, 28.00f, -0.79f, 50.00f, 0.00f, 18.00f, 0.00f, 0.00f, 24.00f, 16.00f, 91.00f, 8.00f, 17.00f, 50.00f, 80.00f, 45.00f, 0.81f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 1.00f);
    presets.emplace_back("Synth Brass", 40.00f, 0.00f, -6.30f, 0.00f, 30.00f, -3.07f, 39.00f, 15.00f, 50.00f, 0.00f, 0.00f, 39.00f, 30.00f, 82.00f, 25.00f, 33.00f, 74.00f, 76.00f, 41.00f, 0.81f, -6.00f, 23.00f, 0.00f, 0.00f, 0.00f, 1.00f);
    presets.emplace_back("Detuned Syn Brass [ZF]", 68.00f, 0.00f, 31.80f, 0.00f, 31.00f, 0.50f, 26.00f, 7.00f, 70.00f, 0.00f, 32.00f, 0.00f, 83.00f, 0.00f, 5.00f, 0.00f, 75.00f, 54.00f, 32.00f, 0.76f, -26.00f, 29.00f, 0.00f, 0.00f, 0.00f, 1.00f);
    presets.emplace_back("Power PWM", 100.00f, -12.00f, -8.80f, 0.00f, 35.00f, 0.00f, 82.00f, 13.00f, 50.00f, 0.00f, -100.00f, 24.00f, 30.00f, 88.00f, 34.00f, 0.00f, 50.00f, 100.00f, 48.00f, 0.71f, -26.00f, 0.00f, -1.00f, 0.00f, 0.00f, 1.00f);
    presets.emplace_back("Water Velocity [SA]", 76.00f, 0.00f, -1.40f, 0.00f, 49.00f, 0.00f, 87.00f, 67.00f, 100.00f, 32.00f, -82.00f, 95.00f, 56.00f, 72.00f, 100.00f, 4.00f, 76.00f, 11.00f, 46.00f, 0.88f, 44.00f, 0.00f, -1.00f, 0.00f, 0.00f, 1.00f);
    presets.emplace_back("Ghost [SA]", 75.00f, 0.00f, -7.10f, 2.00f, 16.00f, -0.00f, 38.00f, 58.00f, 50.00f, 16.00f, 62.00f, 0.00f, 30.00f, 40.00f, 31.00f, 37.00f, 50.00f, 100.00f, 54.00f, 0.85f, 66.00f, 43.00f, 0.00f, 0.00f, 0.00f, 1.00f);
    presets.emplace_back("Soft E.Piano", 31.00f, 0.00f, -0.20f, 0.00f, 35.00f, 0.00f, 34.00f, 26.00f, 6.00f, 0.00f, 26.00f, 0.00f, 22.00f, 0.00f, 39.00f, 0.00f, 80.00f, 0.00f, 44.00f, 0.81f, 2.00f, 0.00f, 0.00f, 0.00f, 0.00f, 1.00f);
    presets.emplace_back("Thumb Piano", 72.00f, 15.00f, 50.00f, 0.00f, 35.00f, 0.00f, 37.00f, 47.00f, 8.00f, 0.00f, 0.00f, 0.00f, 45.00f, 0.00f, 39.00f, 0.00f, 39.00f, 0.00f, 48.00f, 0.81f, 20.00f, 0.00f, 1.00f, 0.00f, 0.00f, 1.00f);
    presets.emplace_back("Steel Drums [ZF]", 81.00f, 12.00f, -12.00f, 0.00f, 18.00f, 2.30f, 40.00f, 30.00f, 8.00f, 17.00f, -20.00f, 0.00f, 42.00f, 23.00f, 47.00f, 12.00f, 48.00f, 0.00f, 49.00f, 0.53f, -28.00f, 34.00f, 0.00f, 0.00f, 0.00f, 1.00f);
    presets.emplace_back("Car Horn", 57.00f, -1.00f, -2.80f, 0.00f, 35.00f, 0.00f, 46.00f, 0.00f, 36.00f, 0.00f, 0.00f, 46.00f, 30.00f, 100.00f, 23.00f, 30.00f, 50.00f, 100.00f, 31.00f, 1.00f, -24.00f, 0.00f, 0.00f, 0.00f, 0.00f, 1.00f);
    presets.emplace_back("Helicopter", 0.00f, -12.00f, 0.00f, 0.00f, 35.00f, 0.00f, 8.00f, 36.00f, 38.00f, 100.00f, 0.00f, 100.00f, 100.00f, 0.00f, 100.00f, 96.00f, 50.00f, 100.00f, 92.00f, 0.97f, 0.00f, 100.00f, -2.00f, 0.00f, 0.00f, 1.00f);
    presets.emplace_back("Arctic Wind", 0.00f, -12.00f, 0.00f, 0.00f, 35.00f, 0.00f, 16.00f, 85.00f, 0.00f, 28.00f, 0.00f, 37.00f, 30.00f, 0.00f, 25.00f, 89.00f, 50.00f, 100.00f, 89.00f, 0.24f, 0.00f, 100.00f, 2.00f, 0.00f, 0.00f, 1.00f);
    presets.emplace_back("Thip", 100.00f, -7.00f, 0.00f, 0.00f, 35.00f, 0.00f, 0.00f, 100.00f, 94.00f, 0.00f, 0.00f, 2.00f, 20.00f, 0.00f, 20.00f, 0.00f, 46.00f, 0.00f, 30.00f, 0.81f, 0.00f, 78.00f, 0.00f, 0.00f, 0.00f, 1.00f);
    presets.emplace_back("Synth Tom", 0.00f, -12.00f, 0.00f, 0.00f, 76.00f, 24.53f, 30.00f, 33.00f, 52.00f, 0.00f, 36.00f, 0.00f, 59.00f, 0.00f, 59.00f, 10.00f, 50.00f, 0.00f, 50.00f, 0.81f, 0.00f, 70.00f, -2.00f, 0.00f, 0.00f, 1.00f);
    presets.emplace_back("Squelchy Frog", 50.00f, -5.00f, -7.90f, 2.00f, 77.00f, -36.00f, 40.00f, 65.00f, 90.00f, 0.00f, 0.00f, 33.00f, 50.00f, 0.00f, 25.00f, 0.00f, 70.00f, 65.00f, 18.00f, 0.32f, 100.00f, 0.00f, -2.00f, 0.00f, 0.00f, 1.00f);
}

const juce::String CX11SynthAudioProcessor::getProgramName(int index) {
  return { presets[index].name };
}

void CX11SynthAudioProcessor::changeProgramName(int index, const juce::String& newName) {
  juce::ignoreUnused(index, newName);
}

void CX11SynthAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock) {
  synth.allocate_resources(sampleRate, samplesPerBlock);
  parametersChanged.store(true); // force update the first time process block is called so the state is initialized.
  reset();
}

void CX11SynthAudioProcessor::releaseResources() {
  synth.deallocate_resources();
}

void CX11SynthAudioProcessor::reset() {
  synth.reset();
}

bool CX11SynthAudioProcessor::isBusesLayoutSupported(
    const BusesLayout& layouts) const {
#if JucePlugin_IsMidiEffect
  juce::ignoreUnused(layouts);
  return true;
#else
  // This is the place where you check if the layout is supported.
  // In this template code we only support mono or stereo.
  // Some plugin hosts, such as certain GarageBand versions, will only
  // load plugins that support stereo bus layouts.
  if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono() &&
      layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
    return false;

    // This checks if the input layout matches the output layout
#if !JucePlugin_IsSynth
  if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
    return false;
#endif

  return true;
#endif
}

void CX11SynthAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) {
  //juce::ignoreUnused(midiMessages);
  juce::ScopedNoDenormals noDenormals;

  auto totalNumInputChannels = getTotalNumInputChannels();
  //auto totalNumOutputChannels = getTotalNumOutputChannels();

  for (auto i = totalNumInputChannels; i < totalNumInputChannels; ++i) {
    buffer.clear(i, 0, buffer.getNumSamples());
  }

  bool expected = true;
  if (isNonRealtime() || parametersChanged.compare_exchange_strong(expected, false)) {
    update();
  }

  splitBufferByEvents(buffer, midiMessages);
}

void CX11SynthAudioProcessor::splitBufferByEvents(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) {
  int bufferOffset = 0;

  for (const auto metadata : midiMessages) {

    // Render audio that happens before this event
    int samplesThisSegment = metadata.samplePosition - bufferOffset;

    if (samplesThisSegment > 0) {
      render(buffer, samplesThisSegment, bufferOffset);
      bufferOffset += samplesThisSegment;
    }

    // Handle the event and ignore some MIDI messages
    if (metadata.numBytes <= 3) {
      uint8_t data1 = (metadata.numBytes >= 2) ? metadata.data[1] : 0;
      uint8_t data2 = (metadata.numBytes == 3) ? metadata.data[2] : 0;
      handleMIDI(metadata.data[0], data1, data2);
    }
  }

  // Render the audio after the last MIDI event. Render the entire buffer
  // if there were no MIDI events.
  int samplesLastSegment = buffer.getNumSamples() - bufferOffset;
  if (samplesLastSegment > 0) {
    render(buffer, samplesLastSegment, bufferOffset);
  }

  midiMessages.clear();
}

void CX11SynthAudioProcessor::update() {
  float noise_mix = noise_param->get() / 100.0f;
  noise_mix *= noise_mix;
  synth.noise_mix = noise_mix * 0.06f;
}

void CX11SynthAudioProcessor::handleMIDI(uint8_t data0, uint8_t data1, uint8_t data2) {
  // Change program via MIDI
  if ((data0 & 0xF0) == 0xC0){
    if (data1 < presets.size()) {
      setCurrentProgram(data1);
    }
  }
  synth.midi_message(data0, data1, data2);
}

void CX11SynthAudioProcessor::render(juce::AudioBuffer<float>& buffer, int sampleCount , int bufferOffset) {
  float* output_buffers[2] = { nullptr, nullptr };
  output_buffers[0] = buffer.getWritePointer(0) + bufferOffset;

  if (getTotalNumOutputChannels() > 1) {
    output_buffers[1] = buffer.getWritePointer(1) + bufferOffset;
  }

  synth.render(output_buffers, sampleCount);
}

bool CX11SynthAudioProcessor::hasEditor() const {
  return true;  // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* CX11SynthAudioProcessor::createEditor() {
  auto editor = new juce::GenericAudioProcessorEditor(*this);
  editor->setSize(500, 1050);
  return editor;
}

void CX11SynthAudioProcessor::getStateInformation(juce::MemoryBlock& destData) {
  // You should use this method to store your parameters in the memory block.
  // You could do that either as raw data, or use the XML or ValueTree classes
  // as intermediaries to make it easy to save and load complex data.
  copyXmlToBinary(*apvts.copyState().createXml(), destData);
}

void CX11SynthAudioProcessor::setStateInformation(const void* data, int sizeInBytes) {
  // You should use this method to restore your parameters from this memory
  // block, whose contents will have been created by the getStateInformation()
  // call.
  std::unique_ptr<juce::XmlElement> xml(getXmlFromBinary(data, sizeInBytes));
  if (xml.get() != nullptr && xml->hasTagName(apvts.state.getType())) {
    apvts.replaceState(juce::ValueTree::fromXml(*xml));
    parametersChanged.store(true);
  }
}

juce::AudioProcessorValueTreeState::ParameterLayout CX11SynthAudioProcessor::createParameterLayout() {
  juce::AudioProcessorValueTreeState::ParameterLayout layout;

  layout.add(std::make_unique<juce::AudioParameterChoice>(
    ParameterId::poly_mode,
    "Polyphony",
    juce::StringArray { "Mono", "Poly" },
    1
  ));

  layout.add(std::make_unique<juce::AudioParameterFloat>(
    ParameterId::osc_tune,
    "Osc Tune",
    juce::NormalisableRange<float>(-24.0f, 24.0f, 1.0f),
    -12.0f,
    juce::AudioParameterFloatAttributes().withLabel("semi")
  ));

  layout.add(std::make_unique<juce::AudioParameterFloat>(
    ParameterId::osc_fine,
    "Osc Fine",
    juce::NormalisableRange<float>(-50.0f, 50.0f, 0.1f, 0.3f, true),
    0.0f,
    juce::AudioParameterFloatAttributes().withLabel("cent")
  ));

  auto osc_mix_string_from_val = [](float value, int)
  {
    char s[16] = {0};
    snprintf(s, 16, "%4.0f:%2.0f", 100.0f - 0.5f * value, 0.5f * value);
    return juce::String(s);
  };

  layout.add(std::make_unique<juce::AudioParameterFloat>(
    ParameterId::osc_mix,
    "Osc Mix",
    juce::NormalisableRange<float>(0.0f, 100.0f),
    0.0f,
    juce::AudioParameterFloatAttributes()
      .withLabel("%")
      .withStringFromValueFunction(osc_mix_string_from_val)
  ));

  layout.add(std::make_unique<juce::AudioParameterChoice>(
    ParameterId::glide_mode,
    "Glide Mode",
    juce::StringArray { "Off", "Legato", "Always" },
    0
  ));

  layout.add(std::make_unique<juce::AudioParameterFloat>(
    ParameterId::glide_rate,
    "Glide Rate",
    juce::NormalisableRange<float>(0.0f, 100.0f, 1.0f),
    35.0f,
    juce::AudioParameterFloatAttributes().withLabel("%")      
  ));

layout.add(std::make_unique<juce::AudioParameterFloat>(
    ParameterId::glide_bend,
    "Glide Bend",
    juce::NormalisableRange<float>(-36.0f, 36.0f, 0.01f, 0.4f, true),
    0.0f,
    juce::AudioParameterFloatAttributes().withLabel("semi")      
  ));

  layout.add(std::make_unique<juce::AudioParameterFloat>(
    ParameterId::filter_freq,
    "Filter Freq",
    juce::NormalisableRange<float>(0.0f, 100.0f, 1.0f),
    100.0f,
    juce::AudioParameterFloatAttributes().withLabel("%")      
  ));

  layout.add(std::make_unique<juce::AudioParameterFloat>(
    ParameterId::filter_reso,
    "Filter Reso",
    juce::NormalisableRange<float>(0.0f, 100.0f, 1.0f),
    15.0f,
    juce::AudioParameterFloatAttributes().withLabel("%")      
  ));

  layout.add(std::make_unique<juce::AudioParameterFloat>(
    ParameterId::filter_env,
    "Filter Env",
    juce::NormalisableRange<float>(-100.0f, 100.0f, 1.0f),
    50.0f,
    juce::AudioParameterFloatAttributes().withLabel("%")      
  ));

  layout.add(std::make_unique<juce::AudioParameterFloat>(
    ParameterId::filter_lfo,
    "Filter LFO",
    juce::NormalisableRange<float>(0.0f, 100.0f, 1.0f),
    0.0f,
    juce::AudioParameterFloatAttributes().withLabel("%")      
  ));

  auto filter_velocity_string_from_val = [](float value, int)
  {
    if (value < -90.0f) {
      return juce::String("OFF");
    } else {
      return juce::String(value);
    }
  };

  layout.add(std::make_unique<juce::AudioParameterFloat>(
    ParameterId::filter_velocity,
    "Filter Velocity",
    juce::NormalisableRange<float>(-100.0f, 100.0f, 1.0f),
    0.0f,
    juce::AudioParameterFloatAttributes()
      .withLabel("%")
      .withStringFromValueFunction(filter_velocity_string_from_val)
  ));

  layout.add(std::make_unique<juce::AudioParameterFloat>(
    ParameterId::filter_attack,
    "Filter Attack",
    juce::NormalisableRange<float>(0.0f, 100.0f, 1.0f),
    0.0f,
    juce::AudioParameterFloatAttributes().withLabel("%")      
  ));

  layout.add(std::make_unique<juce::AudioParameterFloat>(
    ParameterId::filter_decay,
    "Filter Decay",
    juce::NormalisableRange<float>(0.0f, 100.0f, 1.0f),
    30.0f,
    juce::AudioParameterFloatAttributes().withLabel("%")      
  ));

  layout.add(std::make_unique<juce::AudioParameterFloat>(
    ParameterId::filter_sustain,
    "Filter Sustain",
    juce::NormalisableRange<float>(0.0f, 100.0f, 1.0f),
    0.0f,
    juce::AudioParameterFloatAttributes().withLabel("%")      
  ));

  layout.add(std::make_unique<juce::AudioParameterFloat>(
    ParameterId::filter_release,
    "Filter Release",
    juce::NormalisableRange<float>(0.0f, 100.0f, 1.0f),
    25.0f,
    juce::AudioParameterFloatAttributes().withLabel("%")      
  ));

  layout.add(std::make_unique<juce::AudioParameterFloat>(
    ParameterId::env_attack,
    "Env Attack",
    juce::NormalisableRange<float>(0.0f, 100.0f, 1.0f),
    0.0f,
    juce::AudioParameterFloatAttributes().withLabel("%")      
  ));

  layout.add(std::make_unique<juce::AudioParameterFloat>(
    ParameterId::env_decay,
    "Env Decay",
    juce::NormalisableRange<float>(0.0f, 100.0f, 1.0f),
    50.0f,
    juce::AudioParameterFloatAttributes().withLabel("%")      
  ));

  layout.add(std::make_unique<juce::AudioParameterFloat>(
    ParameterId::env_sustain,
    "Env Sustain",
    juce::NormalisableRange<float>(0.0f, 100.0f, 1.0f),
    100.0f,
    juce::AudioParameterFloatAttributes().withLabel("%")      
  ));

  layout.add(std::make_unique<juce::AudioParameterFloat>(
    ParameterId::env_release,
    "Env Release",
    juce::NormalisableRange<float>(0.0f, 100.0f, 1.0f),
    30.0f,
    juce::AudioParameterFloatAttributes().withLabel("%")      
  ));


  auto lfo_rate_string_from_val = [](float value, int)
  {
    float lfoHz = std::exp(7.0f * value -4.0f);
    return juce::String(lfoHz, 3);
  };

  layout.add(std::make_unique<juce::AudioParameterFloat>(
    ParameterId::lfo_rate,
    "LFO Rate",
    juce::NormalisableRange<float>(),
    0.81f,
    juce::AudioParameterFloatAttributes()
      .withLabel("Hz")
      .withStringFromValueFunction(lfo_rate_string_from_val)
  ));

  auto vibrato_string_from_val = [](float value, int)
  {
    if (value < 0.0f) {
      return "PWM " + juce::String(-value - 1);
    } else {
      return juce::String(value, 1);
    }
  };

  layout.add(std::make_unique<juce::AudioParameterFloat>(
    ParameterId::vibrato,
    "Vibrato",
    juce::NormalisableRange<float>(-100.f, 100.f, 0.1f),
    0.0f,
    juce::AudioParameterFloatAttributes()
      .withLabel("%")
      .withStringFromValueFunction(vibrato_string_from_val)
  ));

  layout.add(std::make_unique<juce::AudioParameterFloat>(
    ParameterId::noise,
    "Noise",
    juce::NormalisableRange<float>(0.0f, 100.0f, 1.0f),
    0.0f,
    juce::AudioParameterFloatAttributes().withLabel("%")      
  ));

  layout.add(std::make_unique<juce::AudioParameterFloat>(
    ParameterId::octave,
    "Octave",
    juce::NormalisableRange<float>(-2.0f, 2.0f, 1.0f),
    0.0f
  ));

  layout.add(std::make_unique<juce::AudioParameterFloat>(
    ParameterId::tuning,
    "Tuning",
    juce::NormalisableRange<float>(-100.0f, 100.0f, 1.0f),
    0.0f,
    juce::AudioParameterFloatAttributes().withLabel("cent")      
  ));


  layout.add(std::make_unique<juce::AudioParameterFloat>(
    ParameterId::output_level,
    "Output Level",
    juce::NormalisableRange<float>(-24.0f, 6.0f, 0.1f),
    0.0f,
    juce::AudioParameterFloatAttributes().withLabel("dB")      
  ));


  return layout;
}

}  // namespace audio_plugin

// This creates new instances of the plugin.
// This function definition must be in the global namespace.
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() {
  return new audio_plugin::CX11SynthAudioProcessor();
  //return new CX11SynthAudioProcessor();
}
