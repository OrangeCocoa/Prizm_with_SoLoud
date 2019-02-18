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

		void SetSpeechText(void);

		void Finalize(void)
		{
			_instance.get()->deinit();
		}

		unsigned int AddSound(std::string& file_path, bool do_loop = false)
		{
			SoLoud::Wav sound;
			sound.load(file_path.c_str());
			sound.setLooping(do_loop);

			return _sounds.Load(std::make_shared<SoLoud::Wav>(sound));
		}

		unsigned int AddMusic(std::string& file_path, bool do_loop = false)
		{
			SoLoud::Wav sound;
			sound.load(file_path.c_str());
			sound.setLooping(do_loop);
			sound.setFilter(0, &_filter);

			return _sounds.Load(std::make_shared<SoLoud::Wav>(sound));
		}

		void Set3dMinMaxDistance(unsigned int id, float min_distance, float max_distance)
		{
			_sounds.Get(id)->set3dMinMaxDistance(min_distance, max_distance);
		}

		void Set3dAttenuation(unsigned int id, float rolloff_factor, unsigned int type = SoLoud::AudioSource::EXPONENTIAL_DISTANCE)
		{
			_sounds.Get(id)->set3dAttenuation(type, rolloff_factor);
		}

		void Set3dDopplerFactor(unsigned int id, float doppler_factor)
		{
			_sounds.Get(id)->set3dDopplerFactor(doppler_factor);
		}

		void Set3dListenerRelative(unsigned int id, bool listener_relative)
		{
			_sounds.Get(id)->set3dListenerRelative(listener_relative);
		}

		void Set3dDistanceDelay(unsigned int id, bool distance_delay)
		{
			_sounds.Get(id)->set3dDistanceDelay(distance_delay);
		}

		void Set3dSourceParameters(unsigned int id, float pos_x, float pos_y, float pos_z, float velocity_x, float velocity_y, float velocity_z)
		{
			_instance.get()->set3dSourceParameters(_handle_id[id], pos_x, pos_y, pos_z, velocity_x, velocity_y, velocity_z);
		}

		void Play(unsigned int id)
		{
			SoundHandle sh = _instance.get()->play(*_sounds.Get(id), 1);
			
			_handle_id[id] = sh;
		}

		void Play3d(unsigned int id, float x, float y, float z)
		{
			SoundHandle sh = _instance.get()->play3d(*_sounds.Get(id), x, y, z);

			_handle_id[id] = sh;
		}

		void Stop(unsigned int id)
		{
			_instance.get()->stop(_handle_id[id]);
		}

		void Release(unsigned int id)
		{
			_sounds.Release(id);
		}

		// frequency?
		float *GetWaveShape(void)
		{
			_instance.get()->getWave();
		}

		// calculate spactrum
		float *GetFFT(void)
		{
			_instance.get()->calcFFT();
		}


	};
}