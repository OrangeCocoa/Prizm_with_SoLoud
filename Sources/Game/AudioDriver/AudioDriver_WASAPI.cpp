
#include "AudioDriver_WASAPI.h"

//#include <audiopolicy.h>
//#include <endpointvolume.h>
#include <functiondiscoverykeys.h>

#include"..\..\Utilities\Utils.h"
#include"..\..\Utilities\Log.h"

#pragma comment(lib, "Avrt.lib")

namespace Prizm
{
	const CLSID CLSID_MMDeviceEnumerator = __uuidof(MMDeviceEnumerator);
	const IID IID_IMMDeviceEnumerator = __uuidof(IMMDeviceEnumerator);
	const IID IID_IAudioClient = __uuidof(IAudioClient);
	const IID IID_IAudioRenderClient = __uuidof(IAudioRenderClient);
	const IID IID_IAudioCaptureClient = __uuidof(IAudioCaptureClient);

	bool AudioDriver_WASAPI::Initialize(bool do_capture)
	{
		WAVEFORMATEX* wave_format;
		Microsoft::WRL::ComPtr<IMMDeviceEnumerator> d_enumerator = nullptr;
		Microsoft::WRL::ComPtr<IMMDevice> device = nullptr;

		::CoInitialize(nullptr);

		if (failed(::CoCreateInstance(CLSID_MMDeviceEnumerator, nullptr, CLSCTX_ALL
			, IID_IMMDeviceEnumerator, reinterpret_cast<void**>(d_enumerator.GetAddressOf()))))
		{
			Log::Error("Unable to create WASAPI instance.");
			return false;
		}

		Microsoft::WRL::ComPtr<IMMDeviceCollection> devices = nullptr;
		if (failed(d_enumerator->EnumAudioEndpoints(do_capture ? eCapture : eRender, DEVICE_STATEMASK_ALL, &devices)))
		{
			Log::Error("Unable to enumerate audio endpoints.");
			return false;
		}

		unsigned int device_count = 0;
		if (failed(devices->GetCount(&device_count)))
		{
			Log::Error("Unable to get IMM device count.");
			return false;
		}

		std::wstring device_descriptions;
		bool found = false;

		for (unsigned int i = 0; i < device_count && !found; ++i)
		{
			Microsoft::WRL::ComPtr<IMMDevice> device_temp = nullptr;
	
			if (failed(devices->Item(i, device_temp.GetAddressOf())))
			{
				Log::Error("Unable to get IMM device data.");
				return false;
			}

			Microsoft::WRL::ComPtr<IPropertyStore> props = nullptr;
			if (failed(device_temp->OpenPropertyStore(STGM_READ, props.GetAddressOf())))
			{
				Log::Error("Unable to open device property.");
				return false;
			}

			// get device property to propvar
			PROPVARIANT propvar;
			PropVariantInit(&propvar);
			if (failed(props->GetValue(PKEY_Device_FriendlyName, &propvar)))
			{
				Log::Error("Unable to get device property to propvar.");
				return false;
			}

			device_descriptions.push_back(*propvar.pwszVal);

			PropVariantClear(&propvar);
			props->Release();
			device_temp->Release();
		}



		return true;
	}

	void AudioDriver_WASAPI::Finalize(void)
	{
		::CoUninitialize();
	}
}