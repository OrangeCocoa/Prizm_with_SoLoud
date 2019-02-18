#pragma once

#include<string>

#include <audioclient.h>
#include <mmdeviceapi.h>
#include <windows.h>
#include<wrl\client.h>

#include"AudioDriver.h"

namespace Prizm
{
	class AudioDriver_WASAPI : public AudioDriver
	{
	private:
		struct DeviceWASAPI
		{
			Microsoft::WRL::ComPtr<IAudioClient> _audio_client;
			Microsoft::WRL::ComPtr<IAudioRenderClient> _render_client;
			Microsoft::WRL::ComPtr<IAudioCaptureClient> _capture_client;
			bool _active;

			std::string device_name;

			DeviceWASAPI() 
				: _audio_client(nullptr)
				, _render_client(nullptr)
				, _capture_client(nullptr)
				, _active(false)
				, device_name("Default"){}
		};



	public:
		bool Initialize(bool do_capture);
		void Finalize(void);
	};
}