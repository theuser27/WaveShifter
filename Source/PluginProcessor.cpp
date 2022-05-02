/*
	==============================================================================

		This file contains the basic framework code for a JUCE plugin processor.

	==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
WaveShifterAudioProcessor::WaveShifterAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
	: AudioProcessor(BusesProperties()
	#if ! JucePlugin_IsMidiEffect
	#if ! JucePlugin_IsSynth
		.withInput("Input", juce::AudioChannelSet::stereo(), true)
	#endif
		.withOutput("Output", juce::AudioChannelSet::stereo(), true)
	#endif
	)
#endif
{
	addParameter(shift_ = new AudioParameterFloat("SHIFT", "Shift", -1.0f, 1.0f, 0.0f));
	// if the minValue is too low it will cause crashes in some cases
	addParameter(threshold_ = new AudioParameterFloat("THRESHOLD", "Threshold", 0.01f, 1.0f, 1.0f));
	addParameter(isFiltering_ = new AudioParameterBool("IS_FILTERING_DC", "Is Filtering DC", true));
	//addParameter(isRelativeGain_ = new AudioParameterBool("IS_RELATIVE_GAIN", "Is Relative Gain", false));
	addParameter(bypassThreshold_ = new AudioParameterBool("BYPASS_THRESHOLD", "Bypass Threshold", false));
	addParameter(mode_ = new AudioParameterChoice("MODE", "Mode", {"Clean", "Maximise", "Half", "Broken"}, 0));
}

WaveShifterAudioProcessor::~WaveShifterAudioProcessor()
{
}

//==============================================================================
const juce::String WaveShifterAudioProcessor::getName() const
{
	return JucePlugin_Name;
}

bool WaveShifterAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
	return true;
#else
	return false;
#endif
}

bool WaveShifterAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
	return true;
#else
	return false;
#endif
}

bool WaveShifterAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
	return true;
#else
	return false;
#endif
}

double WaveShifterAudioProcessor::getTailLengthSeconds() const
{
	return 0.0;
}

int WaveShifterAudioProcessor::getNumPrograms()
{
	return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
							// so this should be at least 1, even if you're not really implementing programs.
}

int WaveShifterAudioProcessor::getCurrentProgram()
{
	return 0;
}

void WaveShifterAudioProcessor::setCurrentProgram(int index)
{
}

const juce::String WaveShifterAudioProcessor::getProgramName(int index)
{
	return {};
}

void WaveShifterAudioProcessor::changeProgramName(int index, const juce::String &newName)
{
}

//==============================================================================
void WaveShifterAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
	dcFilter_.init(sampleRate, samplesPerBlock);
}

void WaveShifterAudioProcessor::releaseResources()
{
	// When playback stops, you can use this as an opportunity to free up any
	// spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool WaveShifterAudioProcessor::isBusesLayoutSupported(const BusesLayout &layouts) const
{
#if JucePlugin_IsMidiEffect
	juce::ignoreUnused(layouts);
	return true;
#else
	// This is the place where you check if the layout is supported.
	// In this template code we only support mono or stereo.
	// Some plugin hosts, such as certain GarageBand versions, will only
	// load plugins that support stereo bus layouts.
	if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
		&& layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
		return false;

	// This checks if the input layout matches the output layout
#if ! JucePlugin_IsSynth
	if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
		return false;
#endif

	return true;
#endif
}
#endif

void WaveShifterAudioProcessor::processBlock(juce::AudioBuffer<float> &buffer, juce::MidiBuffer &midiMessages)
{
	juce::ScopedNoDenormals noDenormals;
	auto totalNumInputChannels = getTotalNumInputChannels();
	auto totalNumOutputChannels = getTotalNumOutputChannels();
	jassert(totalNumInputChannels == totalNumOutputChannels);

	// In case we have more outputs than inputs, this code clears any output
	// channels that didn't contain input data, (because these aren't
	// guaranteed to be empty - they may contain garbage).
	// This is here to avoid people getting screaming feedback
	// when they first compile a plugin, but obviously you don't need to keep
	// this code if your algorithm always overwrites all the output channels.
	for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
		buffer.clear(i, 0, buffer.getNumSamples());

	//shifter_.setIsRelativeGain(true);
	
	shifter_.setShift(*shift_);
	shifter_.setThreshold(*threshold_);
	shifter_.setBypassThreshold(*bypassThreshold_);
	shifter_.setAlgorithm(mode_->getIndex());

	// This is the place where you'd normally do the guts of your plugin's
	// audio processing...
	// Make sure to reset the state if your inner loop is processing
	// the samples and the outer loop is handling the channels.
	// Alternatively, you can process the samples with the channels
	// interleaved by keeping the same state.
	shifter_.process(buffer, totalNumInputChannels);
	if (*isFiltering_)
		dcFilter_.filter(buffer, totalNumInputChannels);
	
}

//==============================================================================
bool WaveShifterAudioProcessor::hasEditor() const
{
	return false; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor *WaveShifterAudioProcessor::createEditor()
{
	return new WaveShifterAudioProcessorEditor(*this);
}

//==============================================================================
void WaveShifterAudioProcessor::getStateInformation(juce::MemoryBlock &destData)
{
	// You should use this method to store your parameters in the memory block.
	// You could do that either as raw data, or use the XML or ValueTree classes
	// as intermediaries to make it easy to save and load complex data.
	std::unique_ptr<juce::XmlElement> xml(new juce::XmlElement("MainParams"));
	xml->setAttribute("SHIFT", (double)*shift_);
	xml->setAttribute("THRESHOLD", (double)*threshold_);
	xml->setAttribute("IS_FILTERING", *isFiltering_);
	//xml->setAttribute("IS_RELATIVE_GAIN", *isRelativeGain_);
	xml->setAttribute("BYPASS_THRESHOLD", *bypassThreshold_);
	xml->setAttribute("MODE", mode_->getIndex());
	copyXmlToBinary(*xml, destData);
}

void WaveShifterAudioProcessor::setStateInformation(const void *data, int sizeInBytes)
{
	// You should use this method to restore your parameters from this memory block,
	// whose contents will have been created by the getStateInformation() call.
	std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
	if (xmlState.get() != nullptr)
	{
		if (xmlState->hasTagName("MainParams"))
		{
			*shift_ = (float)xmlState->getDoubleAttribute("SHIFT", 0.0);
			*threshold_ = (float)xmlState->getDoubleAttribute("THRESHOLD", 0.0);
			*isFiltering_ = xmlState->getBoolAttribute("IS_FILTERING", false);
			//*isRelativeGain_ = xmlState->getBoolAttribute("IS_RELATIVE_GAIN", false);
			*bypassThreshold_ = xmlState->getBoolAttribute("BYPASS_THRESHOLD", false);
			*mode_ = xmlState->getIntAttribute("BYPASS_THRESHOLD", 0);
		}
	}
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor *JUCE_CALLTYPE createPluginFilter()
{
	return new WaveShifterAudioProcessor();
}
