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
			// ID�𐶐�
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
			// ���łɖ����Ȃ玸�s
			if (!idAllocator_.CheckActive(id.index, id.version))
			{
				return false;
			}
			Resource<Type>* resource = indexToResource_.Get(id.index);
			// �ǉ�����ĂȂ��L�[�Ȃ玸�s
			if (resource == nullptr)
			{
				return false;
			}
			// �܂��L���ȃ��\�[�X�Ȃ玸�s
			if (resource->IsActive())
			{
				return false;
			}

			// �L�[�Ɨv�f�𖳌�������
			indexToResource_.Remove(id.index);
			idAllocator_.Deallocate(id.index, id.version);

			return true;
		}

		virtual void* Get(ResourceID id)override final
		{
			// ������ID�Ȃ�return
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