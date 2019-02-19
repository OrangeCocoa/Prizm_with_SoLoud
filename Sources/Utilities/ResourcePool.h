#pragma once

#include<vector>
#include<memory>

namespace Prizm
{
	template<class _ResourceType>
	class ResourcePool
	{
	private:
		std::vector<std::shared_ptr<_ResourceType>> _resources;

		// reuce a resource index queue
		std::vector<unsigned int> _reuce;

	public:
		const std::shared_ptr<_ResourceType> &Get(unsigned int index)
		{
			return _resources[index];
		}

		const unsigned int Load(const std::shared_ptr<_ResourceType>& resource)
		{
			if (_reuce.size())
			{
				auto& reuse_index = _reuce.back();
				_reuce.pop_back();

				_resources[reuse_index] = std::move(resource);
				return reuse_index;
			}

			_resources.emplace_back(std::move(resource));
			return static_cast<unsigned int>(_resources.size() - 1);
		}

		void Release(const unsigned int& index)
		{
			_resources[index].reset();
			_reuce.emplace_back(index);
		}

		void Reset(void)
		{
			for (auto& resource : _resources)
			{
				resource = nullptr;
			}

			_resources.clear();
			_reuce.clear();
		}

		const unsigned int Size(void)
		{
			return [&]()
			{
				unsigned int i = 0;
				for (auto& resource : _resources)
				{
					if (resource) ++i;
				}
				return i;
			}();
		}

		bool EmptyResource(void)
		{
			if (_resources.empty()) return true;

			return false;
		}

		bool EmptyReuce(void)
		{
			if (_reuce.empty()) return true;

			return false;
		}

		bool Empty(void)
		{
			if (_resources.empty() && _reuce.empty()) return true;

			return false;
		}
	};
}