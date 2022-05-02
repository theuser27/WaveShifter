/*
	==============================================================================

		WaveShifter.h
		Created: 24 Feb 2022 8:33:33pm
		Author:  Lenovo

	==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class Shifter
{
	static constexpr float kPositiveMax = 1.0f;
	static constexpr float kNegativeMax = -1.0f;

public:
	Shifter() = default;
	~Shifter() = default;

	enum class Algorithms : int
	{
		Clean, Maximise, Half, Broken
	};

	void process(AudioBuffer<float> &buffer, int numChannels)
	{
		int numSamples = buffer.getNumSamples();
		float thresholdValue = (isRelativeGain_) ? threshold_ * buffer.getMagnitude(0, numSamples) : threshold_;

		DBG("shift: " << shift_);
		DBG("threshold: " << thresholdValue);

		for (int channel = 0; channel < numChannels; ++channel)
		{
			auto *channelData = buffer.getWritePointer(channel);
			
			for (int i = 0; i < numSamples; i++)
			{
				channelData[i] = tick(channelData[i], shift_, thresholdValue, algorithm, bypassThreshold_);
			}
		}
	};

	static inline float tick(float value, float shift, float thresholdValue, Algorithms algorithm, bool bypassThreshold = false)
	{
		float shiftedData{ 0.0f };

		switch (algorithm)
		{
		case Shifter::Algorithms::Clean:
			if (!bypassThreshold && (value > thresholdValue || value < -thresholdValue))
				return value;
			shiftedData = value + shift;
			while (shiftedData > thresholdValue)
			{
				// offseting
				shiftedData -= 2.0f * thresholdValue;
			}
			while (shiftedData < -thresholdValue)
			{
				// offseting
				shiftedData += 2.0f * thresholdValue;
			}

			break;
		case Shifter::Algorithms::Maximise:
			if (!bypassThreshold && (value > thresholdValue || value < -thresholdValue))
				return value;
			shiftedData = value + shift;

			if (shiftedData > thresholdValue)
				shiftedData = kNegativeMax;
			else if (shiftedData < -thresholdValue)
				shiftedData = kPositiveMax;

			//shiftedData = (shiftedData > thresholdValue) ? kNegativeMax : shiftedData;
			//shiftedData = (shiftedData < -thresholdValue) ? kPositiveMax : shiftedData;

			break;
		case Shifter::Algorithms::Half:
			if (!bypassThreshold && (value > thresholdValue || value < -thresholdValue))
				return value;
			shiftedData = value + shift;
			shiftedData = (shiftedData > thresholdValue) ? std::fmodf(shiftedData, thresholdValue) - thresholdValue :
				(shiftedData < -thresholdValue) ? std::fmodf(shiftedData, thresholdValue) + thresholdValue :
				shiftedData;

			break;
		case Shifter::Algorithms::Broken:
			if (!bypassThreshold && (value > thresholdValue || value < -thresholdValue))
				return value;
			shiftedData = value + shift;

			break;
		default:
			shiftedData = value;
			break;
		}

		return shiftedData;
	}

	void setShift(float newShift) noexcept { shift_ = newShift; }
	void setThreshold(float newThreshold) noexcept { threshold_ = newThreshold; }
	void setIsRelativeGain(bool newIsRelativeGain) noexcept { isRelativeGain_ = newIsRelativeGain; }
	void setBypassThreshold(bool newIsAbsoluteMode) noexcept { bypassThreshold_ = newIsAbsoluteMode; }
	void setAlgorithm(int newAlgorithm) noexcept { algorithm = Algorithms(newAlgorithm); }

private:
	float shift_{ 0.0f };
	// TODO: separate into upper and lower thresholds
	float threshold_{ 0.0f };
	bool isRelativeGain_{ false };
	bool bypassThreshold_{ false };
	Algorithms algorithm{ Algorithms::Clean };
};