#include "MainComponent.h"

MainComponent::MainComponent()
	: _mainMenuModel(*this)
{
	_editor = uptr<AudioProcessorEditor>(_pluginWrapper.plugin.createEditor());
	_menuBar.setModel(&_mainMenuModel);
	_audioDeviceManager.addAudioCallback(&_pluginWrapper);
	addAndMakeVisible(_editor.get());
	addAndMakeVisible(_menuBar);
}

void MainComponent::paint(Graphics& g)
{
}

void MainComponent::resized()
{
	auto area = getLocalBounds();
	_menuBar.setBounds(area.removeFromTop(20));
	_editor->setBounds(area);
}

MainComponent::MainMenuModel::MainMenuModel(MainComponent& m)
	: _parent(m) {}

MainComponent::MainMenuModel::~MainMenuModel()
{
	if (_deviceManagerWindow != nullptr) delete _deviceManagerWindow;
}

StringArray MainComponent::MainMenuModel::getMenuBarNames()
{
	return StringArray("Settings");
}

PopupMenu MainComponent::MainMenuModel::getMenuForIndex(int topLevelMenuIndex, const String& menuName)
{
	PopupMenu popupMenu;
	switch (topLevelMenuIndex)
	{
	case 0:
		popupMenu.addItem(1, "Manage audio devices");
		break;
	};
	return popupMenu;
}

void MainComponent::MainMenuModel::menuItemSelected(int menuItemID, int topLevelMenuIndex)
{
	switch (topLevelMenuIndex)
	{
	case 0:
		switch (menuItemID)
		{
		case 1:
			_deviceManagerWindow = new DeviceManagerWindow (
				_parent._audioDeviceManager,
				_parent.getLookAndFeel().findColour(ResizableWindow::backgroundColourId));
			break;
		}
		break;
	}
}
