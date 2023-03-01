#pragma once

#include <JuceHeader.h>
#include "../LiveSampler.h"
#include "../PluginProcessor.h"
#include "MainComponent.h"

class MainWindow : public DocumentWindow
{ 
	Colour _background_color = Desktop::getInstance().getDefaultLookAndFeel().findColour(
		ResizableWindow::backgroundColourId
	);
	MainComponent _mainComponent;

public:
	MainWindow(const String& name);
	void closeButtonPressed() override;
private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainWindow);
};
