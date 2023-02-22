#include "PluginEditor.h"

LiveSamplerAudioProcessorEditor::LiveSamplerAudioProcessorEditor(AudioProcessor& p) :
	AudioProcessorEditor(p)
{
	setSize(600, 400);
}

void LiveSamplerAudioProcessorEditor::paint(Graphics& g)
{
	g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
	g.setFont(25.0f);
	g.drawText("Live Sampler", 0, 0, getWidth(), 30, Justification::centred);
}


void LiveSamplerAudioProcessorEditor::resized()
{
}
