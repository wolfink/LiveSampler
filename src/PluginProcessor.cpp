#include "PluginProcessor.h"
#include "PluginEditor.h"

AudioProcessorValueTreeState::ParameterLayout getParameterLayout()
{
	AudioProcessorValueTreeState::ParameterLayout layout;
	layout.add(std::make_unique<AudioParameterFloat>(
		IN_VOLUME_ID, IN_VOLUME_NAME, NormalisableRange<float>(0.0, 1.0, 0.001), 1.0));
	layout.add(std::make_unique<AudioParameterFloat>(
		OUT_VOLUME_ID, OUT_VOLUME_NAME, NormalisableRange<float>(0.0, 1.0, 0.001), 0.0));
	layout.add(std::make_unique<AudioParameterFloat>(
		MIX_ID, MIX_NAME, NormalisableRange<float>(0.0, 100.0, 0.1), 50.0));
	layout.add(std::make_unique<AudioParameterFloat>(
		SHIFT_FACTOR_ID, SHIFT_FACTOR_NAME, NormalisableRange<float>(-1200.0, 1200.0, 1.0), 0.0));
	return layout;
}
LiveSamplerAudioProcessor::LiveSamplerAudioProcessor() :
	AudioProcessor(BusesProperties()
		.withInput ("In" , AudioChannelSet::stereo())
		.withOutput("Out", AudioChannelSet::stereo())),
	_mono(true),
	_pitch_shifters(getNumInputChannels()),
	params(*this, nullptr, "PARAMETERS", getParameterLayout())
{
	_fft = std::make_shared<dsp::FFT>(12);
}

//========================================================================================
const String LiveSamplerAudioProcessor::getName() const
{
	return String();
}

void LiveSamplerAudioProcessor::prepareToPlay(double sampleRate, int maximumExpectedSamplesPerBlock)
{
	for (int i = 0; i < _pitch_shifters.size(); i++) {
		_pitch_shifters[i].prepare(sampleRate, _fft, 4);
	}
}

void LiveSamplerAudioProcessor::releaseResources()
{
}

void LiveSamplerAudioProcessor::processBlock(AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
	auto block_size = buffer.getNumSamples();
	auto buffer_chan_1 = buffer.getWritePointer(1);
	auto num_input_channels = getNumInputChannels();

	// Set parameter values
	float in_volume = PARAMETER(IN_VOLUME_ID);
	float out_volume = PARAMETER(OUT_VOLUME_ID);
	float mix = PARAMETER(MIX_ID) / 100.0;
	float shift = std::pow(2.0, PARAMETER(SHIFT_FACTOR_ID) / 1200.0);

	for (auto metadata : midiMessages) {
		auto message = metadata.getMessage();

		if (message.isNoteOn())
			for (int channel = 0; channel < num_input_channels; channel++)
				_pitch_shifters[channel].createVoice(message.getNoteNumber());
		if (message.isNoteOff())
			for (int channel = 0; channel < num_input_channels; channel++)
				_pitch_shifters[channel].deleteVoice(message.getNoteNumber());

		midi_display.setMidiMessage(message);
	}

	buffer.applyGain(in_volume);

	for (int channel = 0; channel < num_input_channels; channel++) {
		auto channel_buffer = buffer.getWritePointer(channel);
		_pitch_shifters[channel].setMix(mix);
		_pitch_shifters[channel].process(channel_buffer, block_size);
	}


	if (_mono) {
		auto channel_buffer_0 = buffer.getWritePointer(0);
		auto channel_buffer_1 = buffer.getWritePointer(1);
		for (int i = 0; i < block_size; i++) {
			channel_buffer_0[i] = (channel_buffer_0[i] + buffer_chan_1[i]) / 2;
			channel_buffer_1[i] = channel_buffer_0[i];
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
