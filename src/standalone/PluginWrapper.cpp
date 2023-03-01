#include "PluginWrapper.h"

PluginWrapper::PluginWrapper()
{
}

void PluginWrapper::audioDeviceIOCallbackWithContext( const float* const* inputChannelData,
	                                                  int numInputChannels,
	                                                  float* const* outputChannelData,
	                                                  int numOutputChannels,
	                                                  int numSamples,
	                                                  const AudioIODeviceCallbackContext& context)
{
	if (!plugin.isSuspended()) {
		AudioSampleBuffer audio_buffer(outputChannelData, numOutputChannels, 0, numSamples);
		MidiBuffer midi_buffer;
		for (int i = 0; i < numInputChannels; i++)
			audio_buffer.copyFrom(i, 0, inputChannelData[i], numSamples);
		plugin.processBlock(audio_buffer, midi_buffer);
	}
}

void PluginWrapper::audioDeviceAboutToStart(AudioIODevice* device)
{
	plugin.prepareToPlay(device->getCurrentSampleRate(), device->getCurrentBufferSizeSamples());
	plugin.suspendProcessing(false);
}

void PluginWrapper::audioDeviceStopped()
{
	plugin.suspendProcessing(true);
}
