#pragma once
#include "Utility.hpp"

#include "../Registry/ResourceRegistry.h"
#include "../Load/Loader.h"
#include "../View/ResourceView.h"
#include "Pool/ResourcePool.h"

namespace res
{

	class ResourceStorage
	{
	public:
		ResourceStorage(ResourceRegistry& registry, utl::IMemoryAllocator* memAlloc = nullptr);

		template<typename ResType, typename LoaderType, typename...ArgTypes>
		ResourceID Load(utl::IMemoryAllocator* memAlloc, ArgTypes&&...args)
		{
			const utl::TypeID type = registry_.GetTypeID<ResType>();

			// PoolÇ™Ç‹ÇæçÏê¨Ç≥ÇÍÇƒÇ»ÇØÇÍÇŒçÏê¨
			IResourcePool* pool = GetOrCreatePool<ResType>();

			return pool->Load(type, memAlloc, utl::MakeSharedWithAlloc<LoaderType>(memAlloc_.Get(), args...));
			return ResourceID();
		}

		void ReLoad(const ResourceID id)
		{
			IResourcePool* pool = GetPool(id.type);
			if (pool == nullptr)
			{
				return;
			}
			pool->ReLoad(id);
		}

		bool Release(const ResourceID id)
		{
			// LoadÇ≥ÇÍÇƒÇ»Ç¢å^ÇÃÉäÉ\Å[ÉXÇâï˙ÇµÇÊÇ§Ç∆ÇµÇƒÇΩÇÁé∏îs
			utl::UniquePtr<IResourcePool>* pPool = typeToPool_.Get(id.type);
			if (pPool == nullptr)
			{
				return false;
			}

			return pPool->Get()->Get(id);
		}


		template<typename ResType>
		ResourceView<ResType> Look(const ResourceID id)
		{
			Resource<ResType>* resource = GetResource<ResType>(id);
			if (resource == nullptr)
			{
				return ResourceView<ResType>();
			}

			return ResourceView<ResType>(*resource);
		}

	private:

		template<typename ResType>
		Resource<ResType>* GetResource(const ResourceID id)
		{
			const utl::TypeID type = registry_.GetTypeID<ResType>();
			if (type != id.type)
			{
				return nullptr;
			}

			return static_cast<Resource<ResType>*>(typeToPool_[type]->Get(id));
		}

		IResourcePool* GetPool(const utl::TypeID type)
		{
			utl::UniquePtr<IResourcePool>* pPool = typeToPool_.Get(type);
			if (pPool == nullptr)
			{
				return nullptr;
			}

			return pPool->Get();
		}

		template<typename ResType>
		IResourcePool* GetOrCreatePool()
		{
			const utl::TypeID type = registry_.GetTypeID<ResType>();

			IResourcePool* pool = GetPool(type);
			if (pool == nullptr)
			{
				typeToPool_[type] = utl::MakeUniqueWithAlloc<ResourcePool<ResType>>(memAlloc_.Get());
			}

			return typeToPool_[type].Get();
		}

	private:

		ResourceRegistry& registry_;

		utl::MemoryAllocatorHolder memAlloc_;

		utl::DenceMap<utl::UniquePtr<IResourcePool>> typeToPool_;


	};
}