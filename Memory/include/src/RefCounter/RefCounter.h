#pragma once
#include "../Allocator/MemoryAllocator.h"

namespace utl
{
	template<typename Type, typename...ArgTypes>
	class RefCounter
	{
	public:

		RefCounter()
			:ins_(nullptr)
			,memoryAllocator_(nullptr)
			,counter_(0)
		{

		}

		~RefCounter() = default;

		void Release()
		{

		}




	private:

		Type* ins_;

		IMemoryAllocator* memoryAllocator_;

		int counter_;

	};
}
