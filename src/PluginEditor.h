#pragma once

#include <JuceHeader.h>
#include "LiveSampler.h"
#include "PluginProcessor.h"

class LiveSamplerAudioProcessorEditor :
    public AudioProcessorEditor
{
    LiveSamplerAudioProcessor& _audio_processor;

    class LeftPanel : public Component
    {
		LiveSamplerAudioProcessorEditor& _parent;
        Slider _sld_in_volume;
        Slider _sld_mix;
        Label _lbl_in_volume;
        Label _lbl_mix;
        uptr<AudioProcessorValueTreeState::SliderAttachment> _att_in_volume;
        uptr<AudioProcessorValueTreeState::SliderAttachment> _att_mix;

    public:
        LeftPanel(LiveSamplerAudioProcessorEditor&);
        void resized() override;
    } _left_panel;

    class MiddlePanel : public Component
    {
		LiveSamplerAudioProcessorEditor& _parent;
        Slider _sld_pitch_shift;
        Label _lbl_pitch_shift;
        uptr<AudioProcessorValueTreeState::SliderAttachment> _att_pitch_shift;

    public:
        MiddlePanel(LiveSamplerAudioProcessorEditor&);
        void resized() override;
    } _middle_panel;

    class RightPanel : public Component
    {
		LiveSamplerAudioProcessorEditor& _parent;
        Slider _sld_out_volume;
        ComboBox _cmb_mono;
        Label _lbl_out_volume;
        Label _lbl_mono;
        uptr<AudioProcessorValueTreeState::SliderAttachment> _att_out_volume;

    public:
        RightPanel(LiveSamplerAudioProcessorEditor&);
        void resized() override;
    } _right_panel;



public:
    LiveSamplerAudioProcessorEditor(LiveSamplerAudioProcessor&);
    void paint(Graphics&) override;
    void resized() override;
private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LiveSamplerAudioProcessorEditor)
};

