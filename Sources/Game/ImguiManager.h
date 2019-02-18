#pragma once

#include<memory>
#include<string>
#include<DirectXTK/SimpleMath.h>

namespace Prizm
{
	class ImguiManager
	{
	private:
		class Impl;
		std::unique_ptr<Impl> _impl;

	public:
		ImguiManager();
		~ImguiManager();

		void Initialize(void);
		void BeginFrame(void);
		void EndFrame(void);
		void ResizeBegin(void);
		void ResizeEnd(void);
		void Finalize(void);
	};
}