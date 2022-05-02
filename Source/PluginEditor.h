/*
	==============================================================================

		This file contains the basic framework code for a JUCE plugin editor.

	==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class WaveShifterAudioProcessorEditor : public juce::AudioProcessorEditor, public Slider::Listener, public Button::Listener
{
public:
	WaveShifterAudioProcessorEditor(WaveShifterAudioProcessor &);
	~WaveShifterAudioProcessorEditor() override;

	//==============================================================================
	void paint(juce::Graphics &) override;
	void resized() override;

	void sliderValueChanged(Slider *slider) override;
	void buttonClicked(Button *button) override;

private:
	Slider shiftKnob, thresholdKnob;
	ToggleButton isFilteringButton;

	// This reference is provided as a quick way for your editor to
	// access the processor object that created it.
	WaveShifterAudioProcessor &audioProcessor;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WaveShifterAudioProcessorEditor)
};
