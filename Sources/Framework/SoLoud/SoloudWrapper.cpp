
#include<cassert>

#include"SoloudWrapper.h"

namespace Prizm
{
	void SoloudWrapper::Initialize(void)
	{
		_instance = std::make_unique<SoLoud::Soloud>();
		_instance.get()->init(SoLoud::Soloud::CLIP_ROUNDOFF | SoLoud::Soloud::ENABLE_VISUALIZATION | SoLoud::Soloud::LEFT_HANDED_3D);
		_instance.get()->setGlobalVolume(0.75);
		_instance.get()->setPostClipScaler(0.75);
		_filter.setParams(SoLoud::BiquadResonantFilter::LOWPASS, 44100, 100, 10);

		_speech = std::make_unique<SoLoud::Speech>();
	}

	void SoloudWrapper::Finalize(void)
	{
		_instance.get()->deinit();
	}

	unsigned int SoloudWrapper::AddSound(std::string& file_path, bool do_loop = false)
	{
		SoLoud::Wav sound;
		sound.load(file_path.c_str());
		sound.setLooping(do_loop);

		return _sounds.Load(std::make_shared<SoLoud::Wav>(sound));
	}

	unsigned int SoloudWrapper::AddMusic(std::string& file_path, bool do_loop = false)
	{
		SoLoud::Wav sound;
		sound.load(file_path.c_str());
		sound.setLooping(do_loop);
		sound.setFilter(0, &_filter);

		return _sounds.Load(std::make_shared<SoLoud::Wav>(sound));
	}

	void SoloudWrapper::Play(unsigned int id)
	{
		SoundHandle sh = _instance.get()->play(*_sounds.Get(id), 1);

		_handle_id[id] = sh;
	}

	void SoloudWrapper::Play3d(unsigned int id, float x, float y, float z)
	{
		SoundHandle sh = _instance.get()->play3d(*_sounds.Get(id), x, y, z);

		_handle_id[id] = sh;
	}

	void SoloudWrapper::Stop(unsigned int id)
	{
		_instance.get()->stop(_handle_id[id]);
	}

	void SoloudWrapper::Release(unsigned int id)
	{
		_sounds.Release(id);
	}

	void SoloudWrapper::Reset(void)
	{
		_sounds.Reset();
		assert(_sounds.Empty());
	}

	// frequency?
	float* SoloudWrapper::GetWaveShape(void)
	{
		return _instance.get()->getWave();
	}

	// calculate spactrum
	float* SoloudWrapper::GetFFT(void)
	{
		return _instance.get()->calcFFT();
	}
}