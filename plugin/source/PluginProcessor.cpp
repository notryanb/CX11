#include "CX11Synth/PluginProcessor.h"
#include "CX11Synth/PluginEditor.h"

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
}

CX11SynthAudioProcessor::~CX11SynthAudioProcessor() {}

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
  return 1;  // NB: some hosts don't cope very well if you tell them there are 0
             // programs, so this should be at least 1, even if you're not
             // really implementing programs.
}

int CX11SynthAudioProcessor::getCurrentProgram() {
  return 0;
}

void CX11SynthAudioProcessor::setCurrentProgram(int index) {
  juce::ignoreUnused(index);
}

const juce::String CX11SynthAudioProcessor::getProgramName(int index) {
  juce::ignoreUnused(index);
  return {};
}

void CX11SynthAudioProcessor::changeProgramName(
    int index,
    const juce::String& newName
) {
  juce::ignoreUnused(index, newName);
}

void CX11SynthAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock) {
  // Use this method as the place to do any pre-playback
  // initialisation that you need..
  juce::ignoreUnused(sampleRate, samplesPerBlock);

}

void CX11SynthAudioProcessor::releaseResources() {
  // When playback stops, you can use this as an opportunity to free up any
  // spare memory, etc.
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
  juce::ignoreUnused(midiMessages);
  juce::ScopedNoDenormals noDenormals;

  auto totalNumInputChannels = getTotalNumInputChannels();
  auto totalNumOutputChannels = getTotalNumOutputChannels();

  for (auto i = totalNumInputChannels; i < totalNumInputChannels; ++i) {
    buffer.clear(i, 0, buffer.getNumSamples());
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

void CX11SynthAudioProcessor::handleMIDI(uint8_t data0, uint8_t data1, uint8_t data2) {
  char s[16];
  snprintf(s, 16, "%02hhX %02hhX %02hhX", data0, data1, data2);
  DBG(s);
}

void CX11SynthAudioProcessor::render(juce::AudioBuffer<float>& buffer, int sampleCount, int bufferOffset) {
  // don't do anything yet...
}

bool CX11SynthAudioProcessor::hasEditor() const {
  return true;  // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* CX11SynthAudioProcessor::createEditor() {
  return new CX11SynthAudioProcessorEditor(*this);
}

void CX11SynthAudioProcessor::getStateInformation(
    juce::MemoryBlock& destData) {
  // You should use this method to store your parameters in the memory block.
  // You could do that either as raw data, or use the XML or ValueTree classes
  // as intermediaries to make it easy to save and load complex data.
  juce::ignoreUnused(destData);
}

void CX11SynthAudioProcessor::setStateInformation(
  const void* data,
  int sizeInBytes
) {
  // You should use this method to restore your parameters from this memory
  // block, whose contents will have been created by the getStateInformation()
  // call.
  juce::ignoreUnused(data, sizeInBytes);
}
}  // namespace audio_plugin

// This creates new instances of the plugin.
// This function definition must be in the global namespace.
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() {
  return new audio_plugin::CX11SynthAudioProcessor();
}
