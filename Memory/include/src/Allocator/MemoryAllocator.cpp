#include "MemoryAllocator.h"
#include <cstdlib>

namespace utl
{
	void* DefaultMemoryAllocator::Allocate(const size_t size)
	{
		return malloc(size);
	}

	void DefaultMemoryAllocator::Deallocate(void* allocated)
	{
		free(allocated);
	}
	void* MemoryAllocatorHolder::Allocate(const size_t size)
	{
		if (alloc_ == nullptr)
		{
			return malloc(size);
		}
		else
		{
			return alloc_->Allocate(size);
		}
	}
	void MemoryAllocatorHolder::Deallocate(void* allocated)
	{
		if (alloc_ == nullptr)
		{
			free(allocated);
		}
		else
		{
			alloc_->Deallocate(allocated);
		}
	}
}
