#pragma once
#include "Utility.hpp"

namespace ecs
{
	struct EntityID
	{
		uint32_t index;
		uint16_t version;
		uint16_t archID;
	};

	class EntityManager
	{
	public:

		EntityID GenerateEntity(const uint16_t archID)
		{
			EntityID result;

			result.archID = archID;
			idAllocator_.Allocate(result, &EntityID::index, &EntityID::version);

			return result;
		}

		void RemoveEntity(const EntityID entity)
		{
			idAllocator_.Deallocate(entity.index, entity.version);
		}

		bool CheckActive(const EntityID entity)const
		{
			if (!idAllocator_.CheckActive(entity.index, entity.version))
			{
				return false;
			}
			return true;
		}

	private:

		utl::RecyclableIDAllocator idAllocator_;

	};
}


