#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <iostream>

//==============================================================================
// AudioProccessor is where audio is processed, on its own thread.
// The state object in the initializer list contains parameters that can be changed from the safely from the GUI thread.
EqualizerAudioProcessor::EqualizerAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor(BusesProperties()
        .withInput("Input", juce::AudioChannelSet::stereo(), true)
        .withOutput("Output", juce::AudioChannelSet::stereo(), true)
    )
#endif
    , state(*this, nullptr, "STATE", {
        std::make_unique<juce::AudioParameterFloat>("lowpass-cutoff", "Cutoff", 20.f, 20000.f, 20000.f),
        std::make_unique<juce::AudioParameterFloat>("peak-cutoff-1", "Cutoff", 20.f, 20000.f, 20000.f),
        std::make_unique<juce::AudioParameterFloat>("peak-cutoff-2", "Cutoff", 20.f, 20000.f, 20000.f),
        std::make_unique<juce::AudioParameterFloat>("peak-cutoff-3", "Cutoff", 20.f, 20000.f, 20000.f),
        std::make_unique<juce::AudioParameterFloat>("highpass-cutoff", "Cutoff", 20.f, 20000.f, 20.f),
        
        std::make_unique<juce::AudioParameterFloat>("lowpass-bandwidth", "Q", 0.01f, 5.f, 1.f),
        std::make_unique<juce::AudioParameterFloat>("peak-bandwidth-1", "Q", 0.01f, 5.f, 1.f),
        std::make_unique<juce::AudioParameterFloat>("peak-bandwidth-2", "Q", 0.01f, 5.f, 1.f),
        std::make_unique<juce::AudioParameterFloat>("peak-bandwidth-3", "Q", 0.01f, 5.f, 1.f),
        std::make_unique<juce::AudioParameterFloat>("highpass-bandwidth", "Q", 0.01f, 5.f, 1.f),

        std::make_unique<juce::AudioParameterFloat>("peak-gain-1", "Gain", 0.01f, 2.f, 1.f),
        std::make_unique<juce::AudioParameterFloat>("peak-gain-2", "Gain", 0.01f, 2.f, 1.f),
        std::make_unique<juce::AudioParameterFloat>("peak-gain-3", "Gain", 0.01f, 2.f, 1.f),
        
        std::make_unique<juce::AudioParameterBool>("lowpass-bypass", "Bypass", true),
        std::make_unique<juce::AudioParameterBool>("peak-bypass-1", "Bypass", true),
        std::make_unique<juce::AudioParameterBool>("peak-bypass-2", "Bypass", true),
        std::make_unique<juce::AudioParameterBool>("peak-bypass-3", "Bypass", true),
        std::make_unique<juce::AudioParameterBool>("highpass-bypass", "Bypass", true),
    })
{
    // This gives the filter info about what it is about to process.
    // It is given values in the prepareToPlay method.
    spec = juce::dsp::ProcessSpec();

    // Bypass all filters by default.
    filterChain.setBypassed<0>(true);
    filterChain.setBypassed<1>(true);
    filterChain.setBypassed<2>(true);
    filterChain.setBypassed<3>(true);
    filterChain.setBypassed<4>(true);
}

EqualizerAudioProcessor::~EqualizerAudioProcessor()
{
}

//==============================================================================
const juce::String EqualizerAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool EqualizerAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool EqualizerAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool EqualizerAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double EqualizerAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int EqualizerAudioProcessor::getNumPrograms()
{
    return 1;
}

int EqualizerAudioProcessor::getCurrentProgram()
{
    return 0;
}

void EqualizerAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String EqualizerAudioProcessor::getProgramName (int index)
{
    return {};
}

void EqualizerAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

bool EqualizerAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor *EqualizerAudioProcessor::createEditor()
{
    return new EqualizerAudioProcessorEditor(*this);
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool EqualizerAudioProcessor::isBusesLayoutSupported(const BusesLayout &layouts) const
{
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;

    return true;
}
#endif

//==============================================================================
void EqualizerAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    spec.sampleRate = sampleRate;
    spec.numChannels = getNumInputChannels();  
    spec.maximumBlockSize = samplesPerBlock;
  
    filterChain.prepare(spec);
}

void EqualizerAudioProcessor::releaseResources()
{
}



void EqualizerAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // Removes garbage from the buffer in the case that there are unused channels, for ear safety.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    updateFilters();                                            // Updates parameters before processing.
    juce::dsp::AudioBlock<float> block(buffer);                 // Creates an array of pointers to the sample data in the buffer, for some reason..
    juce::dsp::ProcessContextReplacing<float> context(block);   // ProcessContextReplacing tells the processor to overwrite data in the AudioBlock.
    filterChain.process(context);                               // The process method is called sequantially on each filter.
}

void EqualizerAudioProcessor::updateFilters()
// Updates any parameters changed by the GUI.
{
    double sampleRate = getSampleRate();

    filterChain.setBypassed<0>(*state.getRawParameterValue("lowpass-bypass"));
    *filterChain.get<0>().state = *juce::dsp::IIR::Coefficients<float>::makeLowPass(
        sampleRate, 
        *state.getRawParameterValue("lowpass-cutoff"),
        *state.getRawParameterValue("lowpass-bandwidth"));

    filterChain.setBypassed<1>(*state.getRawParameterValue("peak-bypass-1"));
    *filterChain.get<1>().state = *juce::dsp::IIR::Coefficients<float>::makePeakFilter(
        sampleRate,
        *state.getRawParameterValue("peak-cutoff-1"),
        *state.getRawParameterValue("peak-bandwidth-1"),
        *state.getRawParameterValue("peak-gain-1"));

    filterChain.setBypassed<2>(*state.getRawParameterValue("peak-bypass-2"));
    *filterChain.get<2>().state = *juce::dsp::IIR::Coefficients<float>::makePeakFilter(
        sampleRate,
        *state.getRawParameterValue("peak-cutoff-2"),
        *state.getRawParameterValue("peak-bandwidth-2"),
        *state.getRawParameterValue("peak-gain-2"));

    filterChain.setBypassed<3>(*state.getRawParameterValue("peak-bypass-3"));
    *filterChain.get<3>().state = *juce::dsp::IIR::Coefficients<float>::makePeakFilter(
        sampleRate,
        *state.getRawParameterValue("peak-cutoff-3"),
        *state.getRawParameterValue("peak-bandwidth-1"),
        *state.getRawParameterValue("peak-gain-3"));

    filterChain.setBypassed<4>(*state.getRawParameterValue("highpass-bypass"));
    *filterChain.get<4>().state = *juce::dsp::IIR::Coefficients<float>::makeHighPass(
        sampleRate,
        *state.getRawParameterValue("highpass-cutoff"),
        *state.getRawParameterValue("highpass-bandwidth"));
}

//==============================================================================
void EqualizerAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    //juce::MemoryOutputStream(destData, true).writeFloat(*cutoff);
}

void EqualizerAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    //*cutoff = juce::MemoryInputStream(data, static_cast<size_t>(sizeInBytes), false).readFloat();
}

//==============================================================================
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new EqualizerAudioProcessor();
}
