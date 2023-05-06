#pragma once

#include <JuceHeader.h>
#include "Visualizer.h"
#include "juce_dsp/juce_dsp.h"

class EqualizerAudioProcessor  : public juce::AudioProcessor
                            #if JucePlugin_Enable_ARA
                             , public juce::AudioProcessorARAExtension
                            #endif
{
public:
    EqualizerAudioProcessor();
    ~EqualizerAudioProcessor() override;

    //------------------------------------------------------------------------------
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void processBlock(juce::AudioBuffer<float> &, juce::MidiBuffer &) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    //------------------------------------------------------------------------------
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //------------------------------------------------------------------------------
    const juce::String getName() const override;
    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //------------------------------------------------------------------------------
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //------------------------------------------------------------------------------
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    //==============================================================================
    void updateFilters();

    //------------------------------------------------------------------------------
    const juce::NormalisableRange<double> CUTOFF_RANGE = juce::NormalisableRange(20.0, 20000.0, 0.0, 0.5, false);
    juce::AudioProcessorValueTreeState state;
    static enum FiterChainType { LOWPASS, PEAK1, PEAK2, PEAK3, HIGHPASS };

private:
    juce::dsp::ProcessSpec spec;
    juce::NormalisableRange<float> cutoffRange;

    // IIR::Filter can only process a mono signal. ProcessDuplicator allows for multichannel processing.
    using StereoFilter = juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>>;

    // Container for calling processor methods on a group. 
    juce::dsp::ProcessorChain<StereoFilter, StereoFilter, StereoFilter, StereoFilter, StereoFilter> filterChain;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EqualizerAudioProcessor)
};
