#include "PluginProcessor.h"

LiveSamplerAudioProcessor::LiveSamplerAudioProcessor()
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
