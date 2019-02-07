
#define DLL_EXPORT __declspec(dllexport)

#include"Singleton.h"

namespace Prizm
{
	template<class _Template>
	Singleton<_Template>::~Singleton()
	{
		delete self_;
	}

	template<class _Template>
	_Template* Singleton<_Template>::Instance()
	{
		static Singleton<_Template>* p;
		return p;
	}

	template<class _Template>
	Singleton<_Template>::Singleton() : self_(new _Template)
	{
	}
}