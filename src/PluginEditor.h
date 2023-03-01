#pragma once

#include <JuceHeader.h>
#include "LiveSampler.h"
#include "PluginProcessor.h"

class LiveSamplerAudioProcessorEditor :
    public AudioProcessorEditor
{
    LiveSamplerAudioProcessor& _audio_processor;

    class VolumeSlider : public Component
    {
        Slider _slider;
        Label  _label;
        uptr<AudioProcessorValueTreeState::SliderAttachment> _attachment;

    public:
        VolumeSlider(String label_text)
        {
            _slider.setSliderStyle(Slider::SliderStyle::LinearVertical);
            _slider.setTextBoxStyle(Slider::TextBoxBelow, true, 30, 30);
            _slider.setRange(0.0, 1.0, 0.01);
            addAndMakeVisible(_slider);

            _label.setText(label_text, NotificationType::dontSendNotification);
            //_label.attachToComponent(&_slider, false);
            addAndMakeVisible(_label);

        }
        void attach(AudioProcessorValueTreeState& params, String id)
        {
            _attachment = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(params, id, _slider);
        }
        void resized() override
        {
            FlexBox flexBox;
            flexBox.justifyContent = FlexBox::JustifyContent::center;
            flexBox.items.add(FlexItem(_label).withHeight(30));
            flexBox.items.add(FlexItem(_slider).withWidth(30).withMargin(FlexItem::Margin(0, 0, 50, 0)));
            flexBox.performLayout(getLocalBounds());
        }
    } _volumeSlider_in, _volumeSlider_out;

    class MonoBox : public Component
    {
        ComboBox _comboBox;
    public:
        MonoBox(LiveSamplerAudioProcessorEditor* editor)
        {
            _comboBox.addItemList({ "Mono", "Stereo" }, 1);
            _comboBox.setSelectedId(1);
            _comboBox.onChange = [this, editor] {
                editor->_audio_processor.setMono(_comboBox.getSelectedId());
            };
            addAndMakeVisible(_comboBox);
        }
        void resized() override
        {
            FlexBox flexBox;
            flexBox.alignItems = FlexBox::AlignItems::center;
            flexBox.justifyContent = FlexBox::JustifyContent::center;
            flexBox.items.add(FlexItem(_comboBox)
                .withHeight(30)
                .withWidth(80));
            flexBox.performLayout(getLocalBounds());
        }
    } _monoBox;

public:
    LiveSamplerAudioProcessorEditor(LiveSamplerAudioProcessor&);
    void paint(Graphics&) override;
    void resized() override;
};

