#pragma once

#include<windows.h>
#include<string>
#include<memory>

namespace Prizm
{
	template<typename _Type>
	constexpr _Type window_caption = "direct_x_11";

	template<typename _Type>
	constexpr _Type window_width = 1920;

	template<typename _Type>
	constexpr _Type window_height = 1200;

	namespace Window
	{
		bool DLL_EXPORT Initialize(void);
		void DLL_EXPORT Finalize(void);

		HWND DLL_EXPORT GetWindowHandle(void);
	};
}