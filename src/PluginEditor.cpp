#include "PluginEditor.h"

LiveSamplerAudioProcessorEditor::LiveSamplerAudioProcessorEditor(LiveSamplerAudioProcessor& p) :
	AudioProcessorEditor(p),
	_audio_processor(p),
	_right_panel(*this),
	_middle_panel(*this),
	_left_panel(*this)
{
	addAndMakeVisible(_left_panel);
	addAndMakeVisible(_middle_panel);
	addAndMakeVisible(_right_panel);
#if(_DEBUG)
	addAndMakeVisible(_debug_panel);
#endif
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
#if(_DEBUG)
	_debug_panel.setBounds(area.removeFromRight(80));
#endif
	area.removeFromTop(60);
	_left_panel.setBounds(area.removeFromLeft(120));
	_right_panel.setBounds(area.removeFromRight(120));
	_middle_panel.setBounds(area);
}

LiveSamplerAudioProcessorEditor::LeftPanel::LeftPanel(LiveSamplerAudioProcessorEditor& parent) :
	_parent(parent)
{
	_sld_in_volume.setSliderStyle(Slider::SliderStyle::LinearBarVertical);
	_sld_in_volume.setRange(0.0, 1.0, 0.001);
	_att_in_volume = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(
		_parent._audio_processor.params, IN_VOLUME_ID, _sld_in_volume);
	_lbl_in_volume.setText("Input", NotificationType::dontSendNotification);
	_lbl_mix.attachToComponent(&_sld_in_volume, false);
	addAndMakeVisible(_sld_in_volume);

	_sld_mix.setSliderStyle(Slider::SliderStyle::RotaryHorizontalVerticalDrag);
	_sld_mix.setRange(0.0, 100.0, 0.1);
	_sld_mix.setTextValueSuffix("%");
	_att_mix = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(
		_parent._audio_processor.params, MIX_ID, _sld_mix);
	_lbl_mix.setText("Mix", NotificationType::dontSendNotification);
	_lbl_mix.attachToComponent(&_sld_mix, false);
	addAndMakeVisible(_sld_mix);
}

void LiveSamplerAudioProcessorEditor::LeftPanel::resized()
{
	FlexBox flexBox;
	flexBox.flexDirection = FlexBox::Direction::column;
	flexBox.items.add(FlexItem(_sld_mix)
		.withHeight(60)
		.withMargin({ 30, 0, 0, 0 }));
	flexBox.items.add(FlexItem(_sld_in_volume)
		.withFlex(1)
		.withWidth(30)
		.withMargin({ 30, 0, 0, 0 }));
	flexBox.performLayout(getLocalBounds());
}

LiveSamplerAudioProcessorEditor::MiddlePanel::MiddlePanel(LiveSamplerAudioProcessorEditor& parent) :
	_parent(parent)
{
	_sld_shift_factor.setSliderStyle(Slider::SliderStyle::RotaryHorizontalVerticalDrag);
	_sld_shift_factor.setRange(-1200.0, 1200.0, 1.0);
	_sld_shift_factor.setSkewFactorFromMidPoint(1.0);
	_att_shift_factor = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(
		_parent._audio_processor.params, SHIFT_FACTOR_ID, _sld_shift_factor);
	_lbl_pitch_shift.setText("Shift", NotificationType::dontSendNotification);
	_lbl_pitch_shift.attachToComponent(&_sld_shift_factor, true);
	addAndMakeVisible(_sld_shift_factor);

	_midi_display_listener.txt_midi_display.setButtonText("0");
	parent._audio_processor.midi_display.addChangeListener(&_midi_display_listener);
	addAndMakeVisible(_midi_display_listener.txt_midi_display);
}

void LiveSamplerAudioProcessorEditor::MiddlePanel::resized()
{
	FlexBox flexBox;
	flexBox.items.add(FlexItem(_sld_shift_factor)
		.withFlex(1)
		.withHeight(100)
		.withMargin({ 0, 0, 0, 80 }));
	flexBox.items.add(FlexItem(_midi_display_listener.txt_midi_display)
		.withFlex(1)
		.withHeight(40));
	flexBox.performLayout(getLocalBounds());
}

LiveSamplerAudioProcessorEditor::RightPanel::RightPanel(LiveSamplerAudioProcessorEditor& parent) :
	_parent(parent)
{
	_sld_out_volume.setSliderStyle(Slider::SliderStyle::LinearBarVertical);
	_sld_out_volume.setRange(0.0, 1.0, 0.001);
	_att_out_volume = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(
		_parent._audio_processor.params, OUT_VOLUME_ID, _sld_out_volume);
	_lbl_out_volume.setText("Output", NotificationType::dontSendNotification);
	_lbl_out_volume.attachToComponent(&_sld_out_volume, false);
	addAndMakeVisible(_sld_out_volume);

	_cmb_mono.addItemList({ "Mono", "Stereo" }, 1);
	_cmb_mono.setSelectedItemIndex(1);
	addAndMakeVisible(_cmb_mono);
}

void LiveSamplerAudioProcessorEditor::RightPanel::resized()
{
	FlexBox flexBox;
	flexBox.flexDirection = FlexBox::Direction::column;
	flexBox.items.add(FlexItem(_cmb_mono)
		.withHeight(60)
		.withMargin({ 30, 0, 0, 0 }));
	flexBox.items.add(FlexItem(_sld_out_volume)
		.withFlex(1)
		.withWidth(30)
		.withMargin({ 30, 0, 0, 0 }));
	flexBox.performLayout(getLocalBounds());
}


