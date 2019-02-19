
#include"Singleton.h"

namespace Prizm
{
	template<class _Template>
	Singleton<_Template>::~Singleton()
	{
		delete _self;
	}

	template<class _Template>
	_Template* Singleton<_Template>::Instance()
	{
		static Singleton<_Template>* p;
		return p;
	}

	template<class _Template>
	Singleton<_Template>::Singleton() : _self(new _Template)
	{
	}
}