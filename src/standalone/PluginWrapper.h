#include "../LiveSampler.h"
#include "../PluginProcessor.h"

class PluginWrapper : public AudioIODeviceCallback
{
public:
	LiveSamplerAudioProcessor plugin;

	PluginWrapper();

	// Inherited via AudioIODeviceCallback
    virtual void audioDeviceIOCallbackWithContext (const float* const* inputChannelData,
                                                   int numInputChannels,
                                                   float* const* outputChannelData,
                                                   int numOutputChannels,
                                                   int numSamples,
                                                   const AudioIODeviceCallbackContext& context) override;
	virtual void audioDeviceAboutToStart(AudioIODevice* device) override;
	virtual void audioDeviceStopped() override;
private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginWrapper)
};