#pragma once

#include<string>
#include<Windows.h>

/*
Mouse and touch input has raw input data.
These need initialized in WinAPI's event handler.
*/

namespace Prizm
{
	using KeyCode = unsigned int;
	
	namespace Input
	{
		// multi touch num
		constexpr int max_touchcount = 2;

		void DLL_EXPORT Initialize(void);

		// mouse capture
		void DLL_EXPORT CaptureMouse(HWND, bool do_capture);
		bool DLL_EXPORT IsMouseCaptured(void);
		POINT DLL_EXPORT MouseCapturePosition(void);

		// update
		void DLL_EXPORT KeyDown(KeyCode);
		void DLL_EXPORT KeyUp(KeyCode);

		void DLL_EXPORT ButtonDown(KeyCode);
		void DLL_EXPORT ButtonUp(KeyCode);
		void DLL_EXPORT UpdateMousePos(long, long, short);

		void DLL_EXPORT UpdateTouchPos(long, long, int, DWORD);

		// key state
		bool DLL_EXPORT IsKeyPress(const char*);
		
		bool DLL_EXPORT IsKeyReleased(const char*);
		
		bool DLL_EXPORT IsKeyTriggered(const char*);

		// mouse state
		bool DLL_EXPORT IsMousePress(const char*);
		
		bool DLL_EXPORT IsScrollUp(void);
		bool DLL_EXPORT IsScrollDown(void);

		int DLL_EXPORT MouseDeltaX(void);
		int DLL_EXPORT MouseDeltaY(void);

		// touch state
		bool DLL_EXPORT IsTouchPress(int);
		bool DLL_EXPORT IsTouchMove(int);
		bool DLL_EXPORT IsTouchReleased(int);
		bool DLL_EXPORT IsTouchTriggered(int);

		int DLL_EXPORT TouchDeltaX(int);
		int DLL_EXPORT TouchDeltaY(int);

		const long DLL_EXPORT *GetMouseDelta(void);
		const long DLL_EXPORT *GetTouchDelta(int);

		// update end of frame
		void DLL_EXPORT PostStateUpdate(void);
	};
}