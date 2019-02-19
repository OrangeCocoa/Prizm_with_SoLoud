
#include<vector>

#include<Windows.h>

#include"Window.h"

#include"..\Input\Input.h"
#pragma comment(lib, "Input.lib")

#include"..\Utilities\Log.h"
#include"..\Utilities\Utils.h"
#pragma comment(lib, "Utilities.lib")

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace Prizm
{
	namespace Window
	{
		HINSTANCE	_instance;
		HWND		_window_handle;
		int			_screen_width, _screen_height;
		bool		_multi_touch_enable;

		void InitRawInputDevices(void)
		{
			// register touch window for raw input
			/*if (GetSystemMetrics(SM_DIGITIZER) & NID_MULTI_INPUT)
			{
				RegisterTouchWindow(_window_handle, 0);
				_multi_touch_enable = true;
			}*/

			// register mouse for raw input
			// https://msdn.microsoft.com/en-us/library/windows/desktop/ms645565.aspx
			RAWINPUTDEVICE Rid[1];
			Rid[0].usUsagePage = (USHORT)0x01;	// HID_USAGE_PAGE_GENERIC;
			Rid[0].usUsage = (USHORT)0x02;	// HID_USAGE_GENERIC_MOUSE;
			Rid[0].dwFlags = 0;
			Rid[0].hwndTarget = _window_handle;

			// Cast between semantically different integer types : a Boolean type to HRESULT.
			if (FALSE == (RegisterRawInputDevices(Rid, 1, sizeof(RAWINPUTDEVICE))))
			{
				OutputDebugStringA("Failed to register raw input device!");
			}

			// get devices and print info
			//-----------------------------------------------------
			UINT numDevices = 0;
			GetRawInputDeviceList(
				NULL, &numDevices, sizeof(RAWINPUTDEVICELIST));
			if (numDevices == 0) return;

			std::vector<RAWINPUTDEVICELIST> deviceList(numDevices);
			GetRawInputDeviceList(
				&deviceList[0], &numDevices, sizeof(RAWINPUTDEVICELIST));

			std::vector<wchar_t> deviceNameData;
			std::wstring deviceName;
			for (UINT i = 0; i < numDevices; ++i)
			{
				const RAWINPUTDEVICELIST& device = deviceList[i];
				if (device.dwType == RIM_TYPEMOUSE)
				{
					char info[1024];
					sprintf_s(info, "Mouse: Handle=0x%08p\n", device.hDevice);
					OutputDebugStringA(info);

					UINT dataSize = 0;
					GetRawInputDeviceInfoA(
						device.hDevice, RIDI_DEVICENAME, nullptr, &dataSize);
					if (dataSize)
					{
						deviceNameData.resize(dataSize);
						UINT result = GetRawInputDeviceInfoA(
							device.hDevice, RIDI_DEVICENAME, &deviceNameData[0], &dataSize);
						if (result != UINT_MAX)
						{
							deviceName.assign(deviceNameData.begin(), deviceNameData.end());

							char info[1024];
							std::string ndeviceName(deviceName.begin(), deviceName.end());
							sprintf_s(info, "  Name=%s\n", ndeviceName.c_str());
							OutputDebugStringA(info);
						}
					}

					RID_DEVICE_INFO deviceInfo;
					deviceInfo.cbSize = sizeof deviceInfo;
					dataSize = sizeof deviceInfo;
					UINT result = GetRawInputDeviceInfoA(
						device.hDevice, RIDI_DEVICEINFO, &deviceInfo, &dataSize);
					if (result != UINT_MAX)
					{
						char info[1024];
						sprintf_s(info,
							"  Id=%u, Buttons=%u, SampleRate=%u, HorizontalWheel=%s\n",
							deviceInfo.mouse.dwId,
							deviceInfo.mouse.dwNumberOfButtons,
							deviceInfo.mouse.dwSampleRate,
							deviceInfo.mouse.fHasHorizontalWheel ? "1" : "0");
						OutputDebugStringA(info);
					}
				}
			}
		}
	};

	LRESULT __stdcall WndProc(HWND window_handle, UINT message, WPARAM w_param, LPARAM l_param)
	{
		if (ImGui_ImplWin32_WndProcHandler(window_handle, message, w_param, l_param))
			return true;

		switch (message)
		{
		case WM_CREATE:
			break;
		case WM_CLOSE:
			if (MessageBoxA(window_handle, "Quit ?", "User Notification", MB_YESNO | MB_DEFBUTTON2) == IDYES)
			{
				Log::Info("[EXIT] BUTTON DOWN x");
				PostQuitMessage(0);
			}
			break;

			// keyboards
		case WM_KEYDOWN:
			Input::KeyDown(static_cast<KeyCode>(w_param));
			if (Input::IsKeyTriggered("F1") && !Input::IsMouseCaptured()) Input::CaptureMouse(window_handle, true);
			break;

		case WM_KEYUP:
			Input::KeyUp(static_cast<KeyCode>(w_param));
			break;

			// mouse buttons
		case WM_MBUTTONDOWN:
		case WM_LBUTTONDOWN:
		case WM_RBUTTONDOWN:
			if (Input::IsMouseCaptured())	Input::KeyDown(static_cast<KeyCode>(w_param));
			break;

		case WM_MBUTTONUP:
		case WM_RBUTTONUP:
		case WM_LBUTTONUP:
			if (Input::IsMouseCaptured()) Input::KeyUp(static_cast<KeyCode>(w_param));
			break;

		case WM_INPUT:
		{
			UINT raw_input_size = 48;
			BYTE* input_buffer[48];
			memset(input_buffer, 0, raw_input_size);

			GetRawInputData(
				(HRAWINPUT)l_param,
				RID_INPUT,
				input_buffer,
				&raw_input_size,
				sizeof(RAWINPUTHEADER));

			RAWINPUT* raw = (RAWINPUT*)input_buffer;

			if (raw->header.dwType == RIM_TYPEMOUSE && raw->data.mouse.usFlags == MOUSE_MOVE_RELATIVE)
			{
				if (Input::IsMouseCaptured())
				{
					Input::UpdateMousePos(raw->data.mouse.lLastX, raw->data.mouse.lLastY, raw->data.mouse.usButtonData);
					//SetCursorPos(Settings::window_width<int> / 2, Settings::window_height<int> / 2);
				}
			}
			break;
		}
		case WM_TOUCH:
		{
			UINT input_num = LOWORD(w_param);
			TOUCHINPUT* inputs = new TOUCHINPUT[input_num];
			HTOUCHINPUT	input_handle = (HTOUCHINPUT)l_param;
			GetTouchInputInfo(input_handle, input_num, inputs, sizeof(TOUCHINPUT));

			for (unsigned int i = 0; i < input_num; ++i)
			{
				if (i >= Input::max_touchcount) break;

				TOUCHINPUT ti = inputs[i];
				POINT pt;
				pt.x = TOUCH_COORD_TO_PIXEL(ti.x);
				pt.y = TOUCH_COORD_TO_PIXEL(ti.y);
				ScreenToClient(window_handle, &pt);

				Input::UpdateTouchPos(pt.x, pt.y, i, ti.dwFlags);
			}

			delete[] inputs;
			CloseTouchInputHandle(input_handle);
			break;
		}
		default:
			return DefWindowProcA(window_handle, message, w_param, l_param);
		}
		return 0;
	}

	bool Window::Initialize(void)
	{
		WNDCLASSEX  wc;

		memset(&wc, 0, sizeof(wc));

		wc.cbSize = sizeof(wc);
		wc.style = CS_HREDRAW | CS_VREDRAW;
		wc.lpfnWndProc = WndProc;
		wc.hInstance = GetModuleHandleA(nullptr);
		wc.hIcon = LoadIconA(0, IDI_APPLICATION);
		wc.hCursor = LoadCursorA(0, IDC_ARROW);
		wc.hbrBackground = static_cast<HBRUSH>(GetStockObject(DKGRAY_BRUSH));
		wc.lpszClassName = window_caption<LPCSTR>;
		wc.hIconSm = LoadIconA(0, IDI_APPLICATION);

		RegisterClassExA(&wc);

		RECT window_rect = { 0, 0, window_width<long>, window_height<long> };
		AdjustWindowRectEx(&window_rect, WS_OVERLAPPEDWINDOW, true, 0);

		_screen_width = window_rect.right - window_rect.left;
		_screen_height = window_rect.bottom - window_rect.top;

		RECT desktop_rect;
		GetWindowRect(GetDesktopWindow(), &desktop_rect);

		int x = desktop_rect.right < window_rect.right ? 0 : (desktop_rect.right - _screen_width) / 2;
		int y = desktop_rect.bottom < window_rect.bottom ? 0 : (desktop_rect.bottom - _screen_height) / 2;

		_window_handle = CreateWindowExA(0, window_caption<LPCSTR>, window_caption<LPCSTR>,
			WS_OVERLAPPEDWINDOW - (WS_MAXIMIZEBOX + WS_THICKFRAME),
			x, y, _screen_width, _screen_height, 0, 0, GetModuleHandleA(nullptr), 0);

		if (_window_handle == nullptr)
		{
			Log::Error("Can't create window. Window handle is null pointer.");
			PostQuitMessage(0);
			return false;
		}

		InitRawInputDevices();

		ShowWindow(_window_handle, SW_SHOW);

		Log::Info("Window initialize succeeded.");

		return true;
	}

	void Window::Finalize(void)
	{
		DestroyWindow(_window_handle);
		_window_handle = nullptr;

		UnregisterClassA(window_caption<LPCSTR>, _instance);
		_instance = nullptr;

		Log::Info("Window finalize succeeded. Bye~~~");
	}

	HWND Window::GetWindowHandle(void)
	{
		return _window_handle;
	}
}