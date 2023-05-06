#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "Visualizer.h"


//==============================================================================
class EqualizerAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    EqualizerAudioProcessorEditor(EqualizerAudioProcessor &);
    ~EqualizerAudioProcessorEditor() override;

    //==============================================================================
    void paint(juce::Graphics &) override;
    void resized() override;

    //==============================================================================
    //Visualizer visualizer;

private:
    EqualizerAudioProcessor &audioProcessor;

    static const int NUM_OF_FILTERS = 5;
    
    juce::Label cutoffLabels[NUM_OF_FILTERS];
    juce::Slider cutoffSliders[NUM_OF_FILTERS];
    juce::AudioProcessorValueTreeState::SliderAttachment lpCutoffAttachment;
    juce::AudioProcessorValueTreeState::SliderAttachment peakCutoffAttachment1;
    juce::AudioProcessorValueTreeState::SliderAttachment peakCutoffAttachment2;
    juce::AudioProcessorValueTreeState::SliderAttachment peakCutoffAttachment3;
    juce::AudioProcessorValueTreeState::SliderAttachment hpCutoffAttachment;

    juce::Label bandwidthLabels[NUM_OF_FILTERS];
    juce::Slider bandwidthSliders[NUM_OF_FILTERS];
    juce::AudioProcessorValueTreeState::SliderAttachment lpBandwidthAttachment;
    juce::AudioProcessorValueTreeState::SliderAttachment peakBandwidthAttachment1;
    juce::AudioProcessorValueTreeState::SliderAttachment peakBandwidthAttachment2;
    juce::AudioProcessorValueTreeState::SliderAttachment peakBandwidthAttachment3;
    juce::AudioProcessorValueTreeState::SliderAttachment hpBandwidthAttachment;

    juce::Label gainLabels[NUM_OF_FILTERS - 2];
    juce::Slider gainSliders[NUM_OF_FILTERS - 2];
    juce::AudioProcessorValueTreeState::SliderAttachment peakGainAttachment1;
    juce::AudioProcessorValueTreeState::SliderAttachment peakGainAttachment2;
    juce::AudioProcessorValueTreeState::SliderAttachment peakGainAttachment3;

    juce::Label bypassLabels[NUM_OF_FILTERS];
    juce::ToggleButton bypassButtons[NUM_OF_FILTERS];
    juce::AudioProcessorValueTreeState::ButtonAttachment lpBypassAttachment;
    juce::AudioProcessorValueTreeState::ButtonAttachment peakBypassAttachment1;
    juce::AudioProcessorValueTreeState::ButtonAttachment peakBypassAttachment2;
    juce::AudioProcessorValueTreeState::ButtonAttachment peakBypassAttachment3;
    juce::AudioProcessorValueTreeState::ButtonAttachment hpBypassAttachment;




    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EqualizerAudioProcessorEditor)
};
