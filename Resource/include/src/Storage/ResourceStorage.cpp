#include "ResourceStorage.h"


namespace res
{
	ResourceStorage::ResourceStorage(ResourceRegistry& registry, utl::IMemoryAllocator* memAlloc)
		:registry_(registry)
		, memAlloc_(memAlloc)
	{
	}
}