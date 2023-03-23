#pragma once

class DebugPanel : public Component
{
	class VariableDisplay : public ChangeListener
	{
	public:
		TextButton txt_var_display;
		VariableDisplay() {}
		~VariableDisplay() { getDebugVariableBroadcaster().removeChangeListener(this); }
		void changeListenerCallback(ChangeBroadcaster* source) override
		{
			DebugVariableBroadcaster* vb =
				(DebugVariableBroadcaster*) source;
			txt_var_display.setButtonText(String(vb->getVariable()));
		}
	} _variableDisplay;
public:
	DebugPanel()
	{
		_variableDisplay.txt_var_display.setButtonText("0");
		getDebugVariableBroadcaster().addChangeListener(&_variableDisplay);
		addAndMakeVisible(_variableDisplay.txt_var_display);
	}
	void resized() override
	{
		FlexBox flexBox;
		flexBox.items.add(FlexItem(_variableDisplay.txt_var_display).withFlex(1).withHeight(50));
		flexBox.performLayout(getLocalBounds());
	}
private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DebugPanel)
};