#pragma once
#include "../../View/ResourceView.h"
#include "../../Load/Loader.h"

namespace res
{
	class IResourcePool
	{
	public:

		virtual ResourceID Load(
			utl::TypeID typeID,
			utl::IMemoryAllocator* memAlloc,
			utl::SharedPtr<IResourceLoaderVoid> loader
		) = 0;

		virtual void ReLoad(const ResourceID id) = 0;


		virtual bool Release(ResourceID id) = 0;

		virtual void* Get(ResourceID id) = 0;

	private:

	};

	template<typename Type>
	class ResourcePool :public IResourcePool
	{
	public:
		ResourcePool()
		{

		}

		virtual ResourceID Load(
			utl::TypeID typeID,
			utl::IMemoryAllocator* memAlloc,
			utl::SharedPtr<IResourceLoaderVoid> loader
		)override final
		{
			// IDを生成
			ResourceID nID;
			nID.type = static_cast<uint16_t>(typeID);
			idAllocator_.Allocate(nID, &ResourceID::index, &ResourceID::version);

			indexToResource_.Insert(nID.index, memAlloc, loader);

			return nID;
		}

		virtual void ReLoad(const ResourceID id)override final
		{
			Resource<Type>* resource =static_cast<Resource<Type>*>(Get(id));
			if (resource == nullptr)
			{
				return;
			}
			resource->ReLoad();
		}

		virtual bool Release(ResourceID id)override final
		{
			// すでに無効なら失敗
			if (!idAllocator_.CheckActive(id.index, id.version))
			{
				return false;
			}
			Resource<Type>* resource = indexToResource_.Get(id.index);
			// 追加されてないキーなら失敗
			if (resource == nullptr)
			{
				return false;
			}
			// まだ有効なリソースなら失敗
			if (resource->IsActive())
			{
				return false;
			}

			// キーと要素を無効化する
			indexToResource_.Remove(id.index);
			idAllocator_.Deallocate(id.index, id.version);

			return true;
		}

		virtual void* Get(ResourceID id)override final
		{
			// 無効なIDならreturn
			if (!idAllocator_.CheckActive(id.index, id.version))
			{
				return nullptr;
			}

			Resource<Type>* resource = indexToResource_.Get(id.index);
			if (resource == nullptr)
			{
				return nullptr;
			}

			return static_cast<void*>(resource);
		}

	private:

		utl::RecyclableIDAllocator idAllocator_;

		utl::DenceMap<Resource<Type>> indexToResource_;

	};

}