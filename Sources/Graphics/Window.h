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
	constexpr _Type window_height = 1080;

	namespace Window
	{
		bool Initialize(void);
		void Finalize(void);

		HWND GetWindowHandle(void);
	};
}