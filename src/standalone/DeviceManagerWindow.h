#include <JuceHeader.h>
#include "../LiveSampler.h"

class DeviceManagerWindow : public DocumentWindow
{
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DeviceManagerWindow);

	AudioDeviceSelectorComponent _audioDeviceSelectorComponent;

public:
	DeviceManagerWindow(AudioDeviceManager& m, Colour background_color)
		: DocumentWindow("Manage audio devices", background_color, closeButton),
		_audioDeviceSelectorComponent(m, 0, 2, 0, 2, true, true, true, false)
	{
		setOpaque(true);
		setUsingNativeTitleBar(true);
		_audioDeviceSelectorComponent.setSize(600, 400);
		setContentOwned(&_audioDeviceSelectorComponent, true);
		centreWithSize(getWidth(), getHeight());
		setVisible(true);
	};
	void closeButtonPressed() override { delete this; }
};