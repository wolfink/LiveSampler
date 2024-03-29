#pragma once

#include "LiveSampler.h"
#include "PitchShifter.h"

#define PARAMETER(id) *params.getRawParameterValue(id)

class LiveSamplerAudioProcessor : public AudioProcessor
{
	bool _mono;
	std::shared_ptr<dsp::FFT> _fft;
	std::vector<PitchShifter> _pitch_shifters;

public:
	class MidiDisplay : public ChangeBroadcaster
	{
		MidiMessage midiMessage;
	public:
		MidiDisplay() : midiMessage(MidiMessage()) {}
		void setMidiMessage(MidiMessage m)
		{
			midiMessage = m;
			sendChangeMessage();
		}
		MidiMessage getMidiMessage() { return midiMessage; }
	} midi_display;
	AudioProcessorValueTreeState params;
	LiveSamplerAudioProcessor();
	void setMono(bool mono) { _mono = mono; }
	//=================================================================================
	// Inherited from AudioProcessor
	//=================================================================================
	virtual const String getName() const override;
	virtual void prepareToPlay(double sampleRate, int maximumExpectedSamplesPerBlock) override;
	virtual void releaseResources() override;
	virtual void processBlock(AudioBuffer<float>& buffer, MidiBuffer& midiMessages) override;
	virtual double getTailLengthSeconds() const override;
	virtual bool acceptsMidi() const override;
	virtual bool producesMidi() const override;
	virtual AudioProcessorEditor* createEditor() override;
	virtual bool hasEditor() const override;
	virtual int getNumPrograms() override;
	virtual int getCurrentProgram() override;
	virtual void setCurrentProgram(int index) override;
	virtual const String getProgramName(int index) override;
	virtual void changeProgramName(int index, const String& newName) override;
	virtual void getStateInformation(juce::MemoryBlock& destData) override;
	virtual void setStateInformation(const void* data, int sizeInBytes) override;
	//=================================================================================
private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LiveSamplerAudioProcessor)
};
