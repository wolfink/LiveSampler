#pragma once

#include <JuceHeader.h>
#include "PluginEditor.h"

class LiveSamplerAudioProcessor : public AudioProcessor
{
public:
	LiveSamplerAudioProcessor();
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
};
