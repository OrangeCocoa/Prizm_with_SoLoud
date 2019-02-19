
#include"Component.h"

namespace Prizm
{
	void Component::SetOwner(std::shared_ptr<Entity>& entity)
	{
		_owner = entity;
	}

	std::shared_ptr<Entity> Component::GetOwner(void)
	{
		auto owner = _owner.lock();
		return owner;
	}
}