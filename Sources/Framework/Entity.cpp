
#include"Entity.h"

namespace Prizm
{
	void Entity::RunComponets(void)
	{
		for (const auto& component : _components)
		{
			if (component.second)
				component.second->Run();
			else
				_components.erase(component.first);
		}
	}

	void Entity::DrawComponents(void)
	{
		for (const auto& component : _components)
		{
			if (component.second)
				component.second->Draw();
			else
				_components.erase(component.first);
		}
	}

	void Entity::FinalizeComponets(void)
	{
		for (const auto& component : _components)
		{
			if (component.second)
				component.second->Finalize();
			else
				_components.erase(component.first);
		}
	}
}