
#include"SoundSource.h"

namespace Prizm
{
	unsigned int SoundSource::Create(const CriAtomExPlayerHn& player)
	{
		auto index = _sound_sources.Load(std::make_shared<Source>());
		_sound_sources.Get(index)->Create(player);
		return index;
	}

	void SoundSource::Update(unsigned int ss_id, DirectX::SimpleMath::Vector3& pos, DirectX::SimpleMath::Vector3& velocity)
	{
		_sound_sources.Get(ss_id)->Update(pos, velocity);
	}

	void SoundSource::Destroy(unsigned int ss_id)
	{
		_sound_sources.Get(ss_id)->Destroy();
	}

	void SoundSource::DestroyAll(void)
	{
		auto all_object = _sound_sources.Size();

		for (unsigned int i = 0; i < all_object; ++i)
		{
			auto ss = _sound_sources.Get(i);

			if (ss)
			{
				ss->Destroy();
				ss.reset();
			}
		}
	}
}