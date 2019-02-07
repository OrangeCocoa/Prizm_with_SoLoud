
#define DLL_EXPORT __declspec(dllexport)

#include"ResourcePool.h"

namespace Prizm
{
	template<class _ResourceType>
	const std::shared_ptr<_ResourceType>& ResourcePool<_ResourceType>::Get(unsigned int index)
	{
		return resources_[index];
	}

	template<class _ResourceType>
	const unsigned int ResourcePool<_ResourceType>::Load(const std::shared_ptr<_ResourceType>& resource)
	{
		if (reuce_.size())
		{
			auto& reuse_index = reuce_.back();
			reuce_.pop_back();

			resources_[reuse_index] = std::move(resource);
			return reuse_index;
		}

		resources_.emplace_back(std::move(resource));
		return static_cast<unsigned int>(resources_.size() - 1);
	}

	template<class _ResourceType>
	void ResourcePool<_ResourceType>::Release(const unsigned int& index)
	{
		resources_[index] = nullptr;
		reuce_.emplace_back(index);
	}

	template<class _ResourceType>
	const unsigned int ResourcePool<_ResourceType>::Size(void)
	{
		return [&]()
		{
			unsigned int i = 0;
			for (auto& resource : resources_)
			{
				if (resource) ++i;
			}
			return i;
		}();
	}
};