#pragma once

#include<string>
#include<memory>
#include<unordered_map>

#include"SoLoud\soloud.h"
#include"SoLoud\soloud_wav.h"
#include"SoLoud\soloud_speech.h"
#include"SoLoud\soloud_biquadresonantfilter.h"

#include"..\..\Utilities\ResourcePool.h"

#pragma comment(lib, "SoLoud/soloud_static.lib")

namespace Prizm
{
	using SoundHandle = int;

	// singleton
	class SoloudWrapper
	{
	private:
		static SoloudWrapper* _self;
		std::unique_ptr<SoLoud::Soloud> _instance;
		std::unique_ptr<SoLoud::Speech> _speech;
		SoLoud::BiquadResonantFilter _filter;

		ResourcePool<SoLoud::Wav> _sounds;
		std::unordered_map<unsigned int, SoundHandle> _handle_id;

	public:
		void Initialize(void);

		void Finalize(void);

		unsigned int AddSound(std::string& file_path, bool do_loop);

		unsigned int AddMusic(std::string& file_path, bool do_loop);

		void Play(unsigned int id);

		void Play3d(unsigned int id, float x, float y, float z);

		void Stop(unsigned int id);

		void Release(unsigned int id);

		void Reset(void);

		// frequency?
		float *GetWaveShape(void);

		// calculate spactrum
		float *GetFFT(void);


	};
}