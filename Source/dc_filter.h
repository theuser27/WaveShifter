/*
	==============================================================================

		dc_filter.h
		Created: 28 Feb 2022 5:30:44am
		Author:  Lenovo

	==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class DcFilter
{
public:
	DcFilter() = default;
	~DcFilter() = default;

	void init(double sampleRate, int samplesPerBlock)
	{
		sampleRate_ = sampleRate;
		dcCoefficient_ = 1.0f - std::exp2f(-9.0f);
	}

	void filter(AudioBuffer<float> buffer, int numChannels)
	{
		// stereo
		static float pastIn[2];
		static float pastOut[2];

		int numSamples = buffer.getNumSamples();
		for (int channel = 0; channel < numChannels; ++channel)
		{
			auto *channelData = buffer.getWritePointer(channel);
			for (int i = 0; i < numSamples; i++)
			{
				auto temp = channelData[i];
				channelData[i] = channelData[i] - pastIn[channel] + dcCoefficient_ * pastOut[channel];
				pastOut[channel] = channelData[i];
				pastIn[channel] = temp;
			}
		}
	}

private:
	float dcCoefficient_{ 0.0f };
	float sampleRate_{ 44100.0f };
};