#include <JuceHeader.h>
#include "MainWindow.h"

class LiveSamplerApplication : public JUCEApplication
{
	std::unique_ptr<MainWindow> main_window;

public:
    const String getApplicationName() override { return ProjectInfo::projectName; }
	const String getApplicationVersion() override { return ProjectInfo::versionString; }
	void initialise(const String& commandLineParameters) override
	{ 
		main_window = std::make_unique<MainWindow>(getApplicationName());
	}
	void shutdown() override {};
};

START_JUCE_APPLICATION(LiveSamplerApplication)
