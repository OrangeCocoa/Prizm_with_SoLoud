#pragma once

#include<vector>
#include<memory>

namespace Prizm
{
	template<class _ResourceType>
	class ResourcePool
	{
	private:
		std::vector<std::shared_ptr<_ResourceType>> resources_;

		// reuce a resource index queue
		std::vector<unsigned int> reuce_;

	public:
		const std::shared_ptr<_ResourceType> DLL_EXPORT &Get(unsigned int index);

		const unsigned int DLL_EXPORT Load(const std::shared_ptr<_ResourceType>& resource);

		void DLL_EXPORT Release(const unsigned int& index);

		const unsigned int DLL_EXPORT Size(void);
	};
}