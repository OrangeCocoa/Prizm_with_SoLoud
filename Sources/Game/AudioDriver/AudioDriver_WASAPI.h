#pragma once

#include<string>
#include<vector>

#include<audioclient.h>
#include<mmdeviceapi.h>
#include<windows.h>
#include<wrl\client.h>

#include"AudioDriver.h"

namespace Prizm
{
	class AudioDriver_WASAPI : public AudioDriver
	{
	private:
		struct WASAPI_Device
		{
			Microsoft::WRL::ComPtr<IAudioClient>        audio_client;
			Microsoft::WRL::ComPtr<IAudioRenderClient>  render_client;
			Microsoft::WRL::ComPtr<IAudioCaptureClient> capture_client;
			bool active;

			std::string device_name;
			unsigned short format_tag;
			unsigned short bps;
			unsigned int channels;
			unsigned int frame_size;

			WASAPI_Device()
				: audio_client(nullptr)
				, render_client(nullptr)
				, capture_client(nullptr)
				, active(false)
				, format_tag(0)
				, bps(0)
				, channels(0)
				, frame_size(0)
				, device_name("Default"){}
		};

		WASAPI_Device _wasapi_device;
		unsigned int _channels;
		int _buffer_frames;
		int _mix_rate;
		std::vector<int> _samples;
		bool _thread_terminated;

		//static void thread_func(void);

	public:
		bool InitializeAudioDevice(WASAPI_Device* w_device, bool do_capture);
		bool InitializeRenderDevice(void);
		bool Initialize(void);
		void Finalize(void);
	};
}