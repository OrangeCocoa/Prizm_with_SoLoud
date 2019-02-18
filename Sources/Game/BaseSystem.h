#pragma once

#include<memory>
#include<string>

namespace Prizm
{
	class BaseSystem final
	{
	private:
		class Impl;
		std::unique_ptr<Impl> _impl;

	public:
		BaseSystem();
		~BaseSystem();

		bool Initialize(void);
		void Run(void);
		void Finalize(void);
	};
}