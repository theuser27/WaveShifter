/*
	==============================================================================

		This file contains the basic framework code for a JUCE plugin processor.

	==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "shifter.h"
#include "dc_filter.h"

//==============================================================================
/**
*/
class WaveShifterAudioProcessor : public juce::AudioProcessor
{
public:
	//==============================================================================
	WaveShifterAudioProcessor();
	~WaveShifterAudioProcessor() override;

	//==============================================================================
	void prepareToPlay(double sampleRate, int samplesPerBlock) override;
	void releaseResources() override;

#ifndef JucePlugin_PreferredChannelConfigurations
	bool isBusesLayoutSupported(const BusesLayout &layouts) const override;
#endif

	void processBlock(juce::AudioBuffer<float> &, juce::MidiBuffer &) override;

	//==============================================================================
	juce::AudioProcessorEditor *createEditor() override;
	bool hasEditor() const override;

	//==============================================================================
	const juce::String getName() const override;

	bool acceptsMidi() const override;
	bool producesMidi() const override;
	bool isMidiEffect() const override;
	double getTailLengthSeconds() const override;

	//==============================================================================
	int getNumPrograms() override;
	int getCurrentProgram() override;
	void setCurrentProgram(int index) override;
	const juce::String getProgramName(int index) override;
	void changeProgramName(int index, const juce::String &newName) override;

	//==============================================================================
	void getStateInformation(juce::MemoryBlock &destData) override;
	void setStateInformation(const void *data, int sizeInBytes) override;

	DcFilter& getDcFilter() { return dcFilter_; }
	Shifter& getShifter() { return shifter_; }

	void setShift(float newShift) { *shift_ = newShift; }
	void setThreshold(float newThreshold) { *threshold_ = newThreshold; }
	void setIsFiltering(bool newIsFiltering) { *isFiltering_ = newIsFiltering; }

private:
	DcFilter dcFilter_;
	Shifter shifter_;

	// TODO: in-out gain control
	AudioParameterFloat *inGain;
	AudioParameterFloat *outGain;

	AudioParameterFloat *shift_;
	AudioParameterFloat *threshold_;
	AudioParameterBool *isFiltering_;
	//AudioParameterBool *isRelativeGain_;
	AudioParameterBool *bypassThreshold_;
	// TODO: implement in UI
	AudioParameterChoice *mode_;
	//==============================================================================
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WaveShifterAudioProcessor)
};
