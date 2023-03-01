#include "MainWindow.h"

MainWindow::MainWindow(const String& name) :
	DocumentWindow(name, _background_color, allButtons)
{
	setOpaque(true);
	setUsingNativeTitleBar(true);
	_mainComponent.setSize(800, 600);
	setContentOwned(&_mainComponent, true);
	centreWithSize(getWidth(), getHeight());
	setResizable(true, true);
	setVisible(true);
}

void MainWindow::closeButtonPressed()
{
	JUCEApplication::getInstance()->systemRequestedQuit();
}