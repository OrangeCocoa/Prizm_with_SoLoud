#pragma once

namespace Prizm
{
	class Sound
	{
	public:
		enum BACKENDS
		{

			AUTO = 0,
			WINMM,
			XAUDIO2,
			WASAPI,
			NONE,
			BACKEND_MAX,
		};
		/*
		SDL1,
		SDL2,
		PORTAUDIO,
		ALSA,
		OSS,
		OPENAL,
		COREAUDIO,
		OPENSLES,
		VITA_HOMEBREW,
		ASIO,
		*/

	private:
		unsigned int _global_volume;
		unsigned int _sample_rate;
		unsigned int _channels;
		unsigned int _backend_ID;
		unsigned int _backend_string;

		float _visualizaion_data[256];
		float _wav_data[256];
		float _fft_data[256];

		float _3d_position[3];
		float _3d_at[3];
		float _3d_up[3];
		float _3d_velosity[3];

	public:
		Sound(void);
		~Sound(void);

		// return result
		unsigned int Initialize(void);
		void Finalize(void);

		void LockAudioMutex(void);
		void UnlockAudioMutex(void);
	};
}