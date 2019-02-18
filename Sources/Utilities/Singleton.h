#pragma once

namespace Prizm
{
	template<class _Template>
	class Singleton
	{
	private:
		_Template* self_;

	public:
		~Singleton();
		static _Template* Instance();

	private:
		// instance guard
		Singleton();
		Singleton(const Singleton<_Template>&);
		Singleton<_Template>& operator=(const Singleton<_Template>&);
	};
}