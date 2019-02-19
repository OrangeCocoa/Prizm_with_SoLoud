#pragma once

namespace Prizm
{
	template<class _Template>
	class Singleton
	{
	private:
		_Template* _self;

	public:
		~Singleton(void);
		static _Template* Instance(void);

	private:
		// instance guard
		Singleton(void);
		Singleton(const Singleton<_Template>&);
		Singleton<_Template>& operator=(const Singleton<_Template>&) {};
	};
}