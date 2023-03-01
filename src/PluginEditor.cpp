#include "PluginEditor.h"

LiveSamplerAudioProcessorEditor::LiveSamplerAudioProcessorEditor(LiveSamplerAudioProcessor& p) :
	AudioProcessorEditor(p),
	_audio_processor(p),
	_volumeSlider_in("In"),
	_volumeSlider_out("Out"),
	_monoBox(this)
{
	_volumeSlider_in.attach(_audio_processor.params, IN_VOLUME_ID);
	_volumeSlider_out.attach(_audio_processor.params, OUT_VOLUME_ID);

	addAndMakeVisible(_volumeSlider_in);
	addAndMakeVisible(_volumeSlider_out);
	addAndMakeVisible(_monoBox);
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
	auto area = getLocalBounds();
	area.removeFromTop(60);
	_volumeSlider_in.setBounds(area.removeFromLeft(80));
	_volumeSlider_out.setBounds(area.removeFromRight(80));
	_monoBox.setBounds(area);
}
