#pragma once

#include<vector>

namespace Prizm
{
	class AudioDriver
	{
	private:
		static AudioDriver * _self;

		std::vector<int32_t> _input_buffer;
		unsigned int _input_position;
		unsigned int _input_size;

	public:
		enum SpeakerMode
		{
			SPEAKER_MODE_STELEO,
			SPEAKER_MODE_SURROUND_31,
			SPEAKER_MODE_SURROUND_51,
			SPEAKER_MODE_SURROUND_71,
		};

		static const int DEFAULT_MIX_RATE = 44100;
		static const int DEFAULT_OUTPUT_LATENCY = 15;

		static AudioDriver* GetAudioDriver() { return _self; }
	};
}