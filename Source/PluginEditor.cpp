/*
	==============================================================================

		This file contains the basic framework code for a JUCE plugin editor.

	==============================================================================
*/

#include "shifter.h"
#include "dc_filter.h"
#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
WaveShifterAudioProcessorEditor::WaveShifterAudioProcessorEditor(WaveShifterAudioProcessor &p)
	: AudioProcessorEditor(&p), audioProcessor(p)
{
	shiftKnob.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
	shiftKnob.setTextBoxStyle(Slider::TextBoxAbove, false, 40, 0);
	shiftKnob.setRange(-1.0, 1.0, 0.01);
	shiftKnob.setDoubleClickReturnValue(true, 0.0);
	shiftKnob.addListener(this);
	addAndMakeVisible(shiftKnob);

	thresholdKnob.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
	thresholdKnob.setTextBoxStyle(Slider::TextBoxAbove, false, 40, 0);
	thresholdKnob.setRange(0.0, 1.0, 0.01);
	thresholdKnob.setDoubleClickReturnValue(true, 1.0);
	thresholdKnob.addListener(this);
	addAndMakeVisible(thresholdKnob);

	isFilteringButton.setClickingTogglesState(true);
	isFilteringButton.addListener(this);
	addAndMakeVisible(isFilteringButton);

	// Make sure that before the constructor has finished, you've set the
	// editor's size to whatever you need it to be.
	setSize(400, 300);
}

WaveShifterAudioProcessorEditor::~WaveShifterAudioProcessorEditor()
{
}

//==============================================================================
void WaveShifterAudioProcessorEditor::paint(juce::Graphics &g)
{
	// (Our component is opaque, so we must completely fill the background with a solid colour)
	g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

}

void WaveShifterAudioProcessorEditor::resized()
{
	// This is generally where you'll want to lay out the positions of any
	// subcomponents in your editor..
	shiftKnob.setBoundsRelative(0.125f, 0.25f, 0.25f, 0.5f);
	thresholdKnob.setBoundsRelative(0.625f, 0.25f, 0.25f, 0.5f);
	isFilteringButton.setBoundsRelative(0.1f, 0.75f, 0.0675f, 0.0675f);
}

void WaveShifterAudioProcessorEditor::sliderValueChanged(Slider *slider)
{
	if (slider == &shiftKnob)
		audioProcessor.setShift(shiftKnob.getValue());
	else if (slider == &thresholdKnob)
		audioProcessor.setThreshold(thresholdKnob.getValue());
}

void WaveShifterAudioProcessorEditor::buttonClicked(Button *button)
{
	if (button == &isFilteringButton)
		audioProcessor.setIsFiltering(isFilteringButton.getToggleState());
}
