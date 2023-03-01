#include "PluginProcessor.h"
#include "PluginEditor.h"

AudioProcessorValueTreeState::ParameterLayout getParameterLayout()
{
	AudioProcessorValueTreeState::ParameterLayout layout;
	layout.add(std::make_unique<AudioParameterFloat>(
		IN_VOLUME_ID, IN_VOLUME_NAME, NormalisableRange<float>(0.0, 1.0, 0.01), 0.0));
	layout.add(std::make_unique<AudioParameterFloat>(
		OUT_VOLUME_ID, OUT_VOLUME_NAME, NormalisableRange<float>(0.0, 1.0, 0.01), 0.0));
	return layout;
}
LiveSamplerAudioProcessor::LiveSamplerAudioProcessor() :
	AudioProcessor(BusesProperties()
		.withInput ("In" , AudioChannelSet::stereo())
		.withOutput("Out", AudioChannelSet::stereo())),
	_mono(true),
	params(*this, nullptr, "PARAMETERS", getParameterLayout())
{
}

//========================================================================================
const String LiveSamplerAudioProcessor::getName() const
{
	return String();
}

void LiveSamplerAudioProcessor::prepareToPlay(double sampleRate, int maximumExpectedSamplesPerBlock)
{
}

void LiveSamplerAudioProcessor::releaseResources()
{
}

void LiveSamplerAudioProcessor::processBlock(AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
	auto block_size = buffer.getNumSamples();
	auto buffer_chan_0 = buffer.getWritePointer(0);
	auto buffer_chan_1 = buffer.getWritePointer(1);

	float in_volume = PARAMETER(IN_VOLUME);
	float out_volume = PARAMETER(OUT_VOLUME);

	buffer.applyGain(in_volume);

	if (_mono) {
		for (int i = 0; i < block_size; i++) {
			buffer_chan_0[i] = (buffer_chan_0[i] + buffer_chan_1[i]) / 2;
			buffer_chan_1[i] = buffer_chan_0[i];
		}
	}

	buffer.applyGain(out_volume);
}

double LiveSamplerAudioProcessor::getTailLengthSeconds() const
{
	return 0.0;
}

bool LiveSamplerAudioProcessor::acceptsMidi() const
{
	return false;
}

bool LiveSamplerAudioProcessor::producesMidi() const
{
	return false;
}

AudioProcessorEditor* LiveSamplerAudioProcessor::createEditor()
{
	return new LiveSamplerAudioProcessorEditor(*this);
}

bool LiveSamplerAudioProcessor::hasEditor() const
{
	return true;
}

int LiveSamplerAudioProcessor::getNumPrograms()
{
	return 0;
}

int LiveSamplerAudioProcessor::getCurrentProgram()
{
	return 0;
}

void LiveSamplerAudioProcessor::setCurrentProgram(int index)
{
}

const String LiveSamplerAudioProcessor::getProgramName(int index)
{
	return String();
}

void LiveSamplerAudioProcessor::changeProgramName(int index, const String& newName)
{
}

void LiveSamplerAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
}

void LiveSamplerAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
}
//========================================================================================
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
	return new LiveSamplerAudioProcessor();
}
