
#include"SoloudWrapper.h"

namespace Prizm
{
	void SoloudWrapper::Initialize(void)
	{
		_instance = std::make_unique<SoLoud::Soloud>();
		_instance.get()->init(SoLoud::Soloud::CLIP_ROUNDOFF | SoLoud::Soloud::ENABLE_VISUALIZATION);
		_instance.get()->setGlobalVolume(0.75);
		_instance.get()->setPostClipScaler(0.75);
		_filter.setParams(SoLoud::BiquadResonantFilter::LOWPASS, 44100, 100, 10);

		_speech = std::make_unique<SoLoud::Speech>();
	}

	void SoloudWrapper::SetSpeechText(void)
	{

	}
}