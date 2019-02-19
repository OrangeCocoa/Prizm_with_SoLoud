
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

	static bool default_render_device_changed = false;
	static bool default_capture_device_changed = false;

	class NotificationClient : public IMMNotificationClient
	{
	private:
		long _ref;
		Microsoft::WRL::ComPtr<IMMDeviceEnumerator> _enumerator;

	public:
		NotificationClient(void)
			: _ref(1)
			, _enumerator(nullptr) {}

		~NotificationClient(void)
		{
			if (!_enumerator)
				_enumerator.Reset();
		}

		unsigned long __stdcall AddRef(void) 
		{
			return _InterlockedIncrement(&_ref);
		}

		unsigned long __stdcall Release(void)
		{
			unsigned long ulref = _InterlockedDecrement(&_ref);
			if (ulref == 0) delete this;

			return ulref;
		}

		HRESULT __stdcall QueryInterface(const IID& ref_iid, void** ppv_interface)
		{
			if (ref_iid ==IID_IUnknown)
			{
				AddRef();
				*ppv_interface = reinterpret_cast<IUnknown*>(this);
			}
			else if (ref_iid == __uuidof(IMMNotificationClient))
			{
				AddRef();
				*ppv_interface = reinterpret_cast<IMMNotificationClient*>(this);
			}
			else
			{
				*ppv_interface = nullptr;
				return E_NOINTERFACE;
			}

			return S_OK;
		}

		HRESULT __stdcall OnDeviceAdded(LPCWSTR device_id)
		{
			return S_OK;
		};

		HRESULT __stdcall OnDeviceRemoved(LPCWSTR device_id)
		{
			return S_OK;
		}

		HRESULT __stdcall OnDeviceStateChanged(LPCWSTR device_id, DWORD new_state) 
		{
			return S_OK;
		}

		HRESULT __stdcall OnDefaultDeviceChanged(EDataFlow flow, ERole role, LPCWSTR device_id)
		{
			if (role == eConsole)
			{
				if (flow == eRender)
					default_render_device_changed = true;
				else if (flow == eCapture)
					default_capture_device_changed = true;
			}

			return S_OK;
		}

		HRESULT __stdcall OnPropertyValueChanged(LPCWSTR device_id, const PROPERTYKEY key)
		{
			return S_OK;
		}
	};

	static NotificationClient noti_client;

	bool AudioDriver_WASAPI::InitializeAudioDevice(WASAPI_Device* w_device, bool do_capture)
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

		for (auto& device_description : device_descriptions)
		{
			CoTaskMemFree(&device_description);
		}
		
		device_descriptions.clear();

		if (failed(d_enumerator->RegisterEndpointNotificationCallback(&noti_client)))
		{
			Log::Error("WASAPI : RegisterEndpointNotificationCallback error.");
		}

		d_enumerator.Reset();

		device->Activate(IID_IAudioClient, CLSCTX_ALL, nullptr
			, reinterpret_cast<void**>(w_device->audio_client.GetAddressOf()));

		device.Reset();

		if (failed(w_device->audio_client->GetMixFormat(&wave_format)))
		{
			Log::Error("");
			return false;
		}

		w_device->format_tag = wave_format->wFormatTag;
		w_device->bps = wave_format->wBitsPerSample;
		w_device->channels = wave_format->nChannels;
		w_device->frame_size = (w_device->bps / 8) * w_device->channels;

		if (w_device->format_tag == WAVE_FORMAT_EXTENSIBLE)
		{
			WAVEFORMATEXTENSIBLE* wave_form_ex = reinterpret_cast<WAVEFORMATEXTENSIBLE*>(wave_format);

			if (wave_form_ex->SubFormat == KSDATAFORMAT_SUBTYPE_PCM)
				w_device->format_tag = WAVE_FORMAT_PCM;
			else if (wave_form_ex->SubFormat == KSDATAFORMAT_SUBTYPE_IEEE_FLOAT)
				w_device->format_tag = WAVE_FORMAT_IEEE_FLOAT;
			else
			{
				Log::Error("WASAPI format not supported.");
				return false;
			}
		}
		else if (w_device->format_tag != WAVE_FORMAT_PCM && w_device->format_tag != WAVE_FORMAT_IEEE_FLOAT)
		{
			Log::Error("WASAPI format not supported.");
			return false;
		}

		DWORD stream_flag;
		if (_mix_rate != wave_format->nSamplesPerSec)
		{
			stream_flag |= AUDCLNT_STREAMFLAGS_RATEADJUST;
			wave_format->nSamplesPerSec = _mix_rate;
			wave_format->nAvgBytesPerSec = wave_format->nSamplesPerSec * wave_format->nChannels * (wave_format->wBitsPerSample / 8);
		}

		if (failed(w_device->audio_client->Initialize(AUDCLNT_SHAREMODE_SHARED, stream_flag, do_capture ? 10000000 : 0, 0, wave_format, nullptr)))
		{
			Log::Error("WASAPI audio client initialize failed.");
			return false;
		}

		HRESULT hr;
		if (do_capture)
			hr = w_device->audio_client->GetService(IID_IAudioCaptureClient, reinterpret_cast<void **>(w_device->capture_client.GetAddressOf()));
		else
			hr = w_device->audio_client->GetService(IID_IAudioRenderClient, reinterpret_cast<void **>(w_device->render_client.GetAddressOf()));

		if (failed(hr))
		{
			Log::Error("WASAPI audio client capture failed.");
			return false;
		}

		CoTaskMemFree(wave_format);
		device.Reset();

		return true;
	}

	bool AudioDriver_WASAPI::InitializeRenderDevice(void)
	{
		if (!InitializeAudioDevice(&_wasapi_device, false)) return false;

		switch (_wasapi_device.channels)
		{
		case 2:
		case 4:
		case 6:
		case 8:
			_channels = _wasapi_device.channels;
		default:
			_channels = 2;
			Log::Warning("WASAPI unsupported number of channels.");
		}

		unsigned int max_frames;
		if (failed(_wasapi_device.audio_client->GetBufferSize(&max_frames)))
		{
			Log::Error("Failed get audio client frame buffer");
			return false;
		}

		_buffer_frames = max_frames;

		_samples.resize(_buffer_frames * _channels);

		Log::Info("WASAPI detected " + std::to_string(_channels) + " channels.");
		Log::Info("WASAPI audio buffer " + std::to_string(_buffer_frames) 
			+ " frames, and calculated latency " + std::to_string(_buffer_frames * 1000 / _mix_rate) + "ms.");

		return true;
	}

	bool AudioDriver_WASAPI::Initialize(void)
	{
		_mix_rate = DEFAULT_MIX_RATE;

		//if (!InitializeRenderDevice()) return false;

		// mutex and thread registration

		return true;
	}

	void AudioDriver_WASAPI::Finalize(void)
	{
		::CoUninitialize();
	}
}