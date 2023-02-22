#include "MainWindow.h"

MainWindow::MainWindow(const String& name) :
	DocumentWindow(name, background_color, allButtons)
{
	setOpaque(true);
	setUsingNativeTitleBar(true);
	audio_processor = std::make_unique<LiveSamplerAudioProcessor>();
	setContentOwned(audio_processor->createEditor(), true);
	setResizable(true, true);
	centreWithSize(getWidth(), getHeight());
	setVisible(true);
}

void MainWindow::closeButtonPressed()
{
	JUCEApplication::getInstance()->systemRequestedQuit();
}