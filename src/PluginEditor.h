#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

class LiveSamplerAudioProcessorEditor :
    public AudioProcessorEditor
{
public:
    LiveSamplerAudioProcessorEditor(AudioProcessor&);
    void paint(Graphics&) override;
    void resized() override;
};

