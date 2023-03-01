#include <JuceHeader.h>
#include "../LiveSampler.h"
#include "../PluginProcessor.h"
#include "PluginWrapper.h"
#include "DeviceManagerWindow.h"


class MainComponent : public Component
{
	PluginWrapper _pluginWrapper;
	AudioDeviceManager _audioDeviceManager;
	uptr<AudioProcessorEditor> _editor;
	MenuBarComponent _menuBar;

	class MainMenuModel : public MenuBarModel
	{
		DeviceManagerWindow* _deviceManagerWindow;
		MainComponent& _parent;
	public:
		MainMenuModel(MainComponent&);
		~MainMenuModel();

		// Inherited via MenuBarModel
		virtual StringArray getMenuBarNames() override;
		virtual PopupMenu getMenuForIndex(int topLevelMenuIndex, const String& menuName) override;
		virtual void menuItemSelected(int menuItemID, int topLevelMenuIndex) override;
	} _mainMenuModel;

public:
	MainComponent();

	// Inherited via Component
	void paint(Graphics &g) override;
	void resized() override;
};