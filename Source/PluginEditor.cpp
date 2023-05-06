#include "PluginProcessor.h"
#include "PluginEditor.h"

// These do not have default constructors. This is to make the class initializer list look cleaner.
#define ATTACHMENTS \
    lpCutoffAttachment(p.state, "lowpass-cutoff", cutoffSliders[0]),\
    peakCutoffAttachment1(p.state, "peak-cutoff-1", cutoffSliders[1]),\
    peakCutoffAttachment2(p.state, "peak-cutoff-2", cutoffSliders[2]),\
    peakCutoffAttachment3(p.state, "peak-cutoff-3", cutoffSliders[3]),\
    hpCutoffAttachment(p.state, "highpass-cutoff", cutoffSliders[4]),\
    \
    lpBandwidthAttachment(p.state, "lowpass-bandwidth", bandwidthSliders[0]),\
    peakBandwidthAttachment1(p.state, "peak-bandwidth-1", bandwidthSliders[1]),\
    peakBandwidthAttachment2(p.state, "peak-bandwidth-2", bandwidthSliders[2]),\
    peakBandwidthAttachment3(p.state, "peak-bandwidth-3", bandwidthSliders[3]),\
    hpBandwidthAttachment(p.state, "highpass-bandwidth", bandwidthSliders[4]),\
    \
    peakGainAttachment1(p.state, "peak-gain-1", gainSliders[0]),\
    peakGainAttachment2(p.state, "peak-gain-2", gainSliders[1]),\
    peakGainAttachment3(p.state, "peak-gain-3", gainSliders[2]),\
    \
    lpBypassAttachment(p.state, "lowpass-bypass", bypassButtons[0]),\
    peakBypassAttachment1(p.state, "peak-bypass-1", bypassButtons[1]),\
    peakBypassAttachment2(p.state, "peak-bypass-2", bypassButtons[2]),\
    peakBypassAttachment3(p.state, "peak-bypass-3", bypassButtons[3]),\
    hpBypassAttachment(p.state, "highpass-bypass", bypassButtons[4])
    

//==============================================================================
// AudioProccessorEditor is where the GUI is handled.
// The initializer list contains attachments that allow for thread safe communication between the GUI and AudioProcessor.
EqualizerAudioProcessorEditor::EqualizerAudioProcessorEditor(EqualizerAudioProcessor &p) :
    AudioProcessorEditor(&p), 
    audioProcessor(p),
    ATTACHMENTS
{
    setSize(720, 250);

    //addAndMakeVisible(&visualizer);
    //visualizer.setSamplesPerBlock(audioProcessor.getBlockSize());
    //visualizer.setRepaintRate(30);
    
    for (int i = 0; i < NUM_OF_FILTERS; i++)
    {
        addAndMakeVisible(&bypassButtons[i]);

        addAndMakeVisible(&bypassLabels[i]);
        bypassLabels[i].setFont(12.f);
        bypassLabels[i].setText("Bypass", juce::dontSendNotification);
        bypassLabels[i].attachToComponent(&bypassButtons[i], true);

        addAndMakeVisible(&cutoffLabels[i]);
        cutoffLabels[i].setFont(12.f);
        cutoffLabels[i].setText("Cutoff", juce::dontSendNotification);

        addAndMakeVisible(&cutoffSliders[i]);
        cutoffSliders[i].setSliderStyle(juce::Slider::LinearBarVertical);
        cutoffSliders[i].setNormalisableRange(audioProcessor.CUTOFF_RANGE);
        cutoffSliders[i].setTextBoxStyle(juce::Slider::NoTextBox, false, 90, 0);
        cutoffSliders[i].setPopupDisplayEnabled(true, false, this);
        cutoffSliders[i].setTextValueSuffix(" Hz");
        if (i == 4) cutoffSliders[i].setValue(20.f);
        else cutoffSliders[i].setValue(20000.f);

        addAndMakeVisible(bandwidthLabels[i]);
        bandwidthLabels[i].setFont(12.f);
        bandwidthLabels[i].setText("Q", juce::dontSendNotification);

        addAndMakeVisible(&bandwidthSliders[i]);
        bandwidthSliders[i].setSliderStyle(juce::Slider::LinearBarVertical);
        bandwidthSliders[i].setNormalisableRange(juce::NormalisableRange<double>(0.01, 5.0));
        bandwidthSliders[i].setTextBoxStyle(juce::Slider::NoTextBox, false, 90, 0);
        bandwidthSliders[i].setPopupDisplayEnabled(true, false, this);
        bandwidthSliders[i].setValue(1.f);

        if (i < NUM_OF_FILTERS - 2)
        {
            addAndMakeVisible(gainLabels[i]);
            gainLabels[i].setFont(12.f);
            gainLabels[i].setText("Gain", juce::dontSendNotification);

            addAndMakeVisible(&gainSliders[i]);
            gainSliders[i].setSliderStyle(juce::Slider::LinearBarVertical);
            gainSliders[i].setNormalisableRange(juce::NormalisableRange<double>(0.01, 2.0));
            gainSliders[i].setTextBoxStyle(juce::Slider::NoTextBox, false, 90, 0);
            gainSliders[i].setPopupDisplayEnabled(true, false, this);
            gainSliders[i].setValue(1.f);
        }
    }
}

EqualizerAudioProcessorEditor::~EqualizerAudioProcessorEditor()
{
}

//==============================================================================
void EqualizerAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
    g.setColour(juce::Colours::whitesmoke);

    juce::Rectangle<int> border(getWidth() / NUM_OF_FILTERS, getHeight());
    for (int i = 0; i < 5; i++)
    {
        border.setPosition(border.getWidth() * i, 0);
        g.drawRect(border);
    }

    //visualizer.pushBuffer(audioProcessor.currentBuffer);
}

void EqualizerAudioProcessorEditor::resized()
// This is where GUI layout is done. 
// I used JUCE's recommended method of creating a layout using rectangles.
{
    juce::Rectangle<int> canvas = getLocalBounds();
    const int MARGIN = 10;
    const int FILTER_BOX_WIDTH = canvas.getWidth() / NUM_OF_FILTERS;
    const int SLIDER_WIDTH = FILTER_BOX_WIDTH / 5;
    canvas.removeFromLeft(MARGIN);
    canvas.removeFromRight(MARGIN);
    canvas.removeFromTop(MARGIN);
    canvas.removeFromBottom(MARGIN);

    //visualizer.setBounds(canvas.removeFromTop(canvas.getHeight() / 2));   

    for (int i = 0; i < NUM_OF_FILTERS; i++)
    {
        juce::Rectangle<int> labelBox(FILTER_BOX_WIDTH, bandwidthLabels[i].getFont().getHeight());
        juce::Rectangle<int> sliderBox(FILTER_BOX_WIDTH, canvas.getHeight() - labelBox.getHeight());
        sliderBox.setPosition(canvas.getPosition());
        labelBox.setPosition(sliderBox.getBottomLeft());

        cutoffLabels[i].setSize(FILTER_BOX_WIDTH / 3, labelBox.getHeight());
        cutoffLabels[i].setTopLeftPosition(labelBox.getPosition().getX() - 5, labelBox.getPosition().getY());
        labelBox.removeFromLeft(FILTER_BOX_WIDTH / 3);

        cutoffSliders[i].setSize(SLIDER_WIDTH, sliderBox.getHeight());
        cutoffSliders[i].setTopLeftPosition(sliderBox.getPosition());
        sliderBox.removeFromLeft(FILTER_BOX_WIDTH / 3);

        juce::Rectangle<int> sliderSubBox(FILTER_BOX_WIDTH - (FILTER_BOX_WIDTH / 3), canvas.getHeight() - labelBox.getHeight());
        sliderSubBox.setPosition(sliderBox.getPosition());

        bypassButtons[i].setSize(30, 30);
        auto box = sliderSubBox.removeFromTop(bypassButtons[i].getHeight());
        bypassButtons[i].setBoundsToFit(box.removeFromLeft(bypassButtons[i].getWidth() + 40), juce::Justification::centredRight, false);

        bandwidthLabels[i].setSize(FILTER_BOX_WIDTH / 3, labelBox.getHeight());
        bandwidthLabels[i].setTopLeftPosition(labelBox.getPosition().getX() + 5, labelBox.getPosition().getY());
        labelBox.removeFromLeft(FILTER_BOX_WIDTH / 3);

        bandwidthSliders[i].setSize(SLIDER_WIDTH, sliderSubBox.getHeight());
        bandwidthSliders[i].setTopLeftPosition(sliderSubBox.getPosition());
        sliderSubBox.removeFromLeft(FILTER_BOX_WIDTH / 3);

        if ((i != 0) && (i < (NUM_OF_FILTERS - 1)))
        {
            gainLabels[i - 1].setSize(FILTER_BOX_WIDTH / 3, labelBox.getHeight());
            gainLabels[i - 1].setTopLeftPosition(labelBox.getPosition().getX() - 5, labelBox.getPosition().getY());

            gainSliders[i - 1].setSize(SLIDER_WIDTH, sliderSubBox.getHeight());
            gainSliders[i - 1].setTopLeftPosition(sliderSubBox.getPosition());
        }

        canvas.removeFromLeft(FILTER_BOX_WIDTH);
    }
}
