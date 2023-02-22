#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

class MainWindow : public DocumentWindow
{ 
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainWindow);

	Colour background_color = Desktop::getInstance().getDefaultLookAndFeel().findColour(
		ResizableWindow::backgroundColourId
	);
	std::unique_ptr<LiveSamplerAudioProcessor> audio_processor;

public:
	MainWindow(const String& name);
	void closeButtonPressed() override;
};
