
#include"SoundSource.h"

namespace Prizm
{
	unsigned int SoundSource::Create(const CriAtomExPlayerHn& player)
	{
		auto index = sound_sources_.Load(std::make_shared<Source>());
		sound_sources_.Get(index)->Create(player);
		return index;
	}

	void SoundSource::Update(unsigned int ss_id, DirectX::SimpleMath::Vector3& pos, DirectX::SimpleMath::Vector3& velocity)
	{
		sound_sources_.Get(ss_id)->Update(pos, velocity);
	}

	void SoundSource::Destroy(unsigned int ss_id)
	{
		sound_sources_.Get(ss_id)->Destroy();
	}

	void SoundSource::DestroyAll(void)
	{
		auto all_object = sound_sources_.Size();

		for (unsigned int i = 0; i < all_object; ++i)
		{
			auto ss = sound_sources_.Get(i);

			if (ss)
			{
				ss->Destroy();
				ss.reset();
			}
		}
	}
}