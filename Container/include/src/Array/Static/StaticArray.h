#pragma once
#include "Memory.hpp"

namespace utl
{
	template<typename Type, size_t SIZE>
	class StaticArray
	{
	public:
		StaticArray(IMemoryAllocator* alloc = nullptr)
			: alloc_(alloc)
			, begin_(alloc_.Allocate(SIZE * sizeof(Type)))
		{
			for (size_t i = 0; i < SIZE; i++)
			{
				Type* ptr = begin_ + i;
				new (ptr) Type();
			}
		}

		~StaticArray()
		{
			for (size_t i = 0; i < SIZE; i++)
			{
				Get(i)->~Type();
			}

			alloc_.Deallocate(begin_);
			begin_ = nullptr;
		}

		Type& operator[](const size_t index)const
		{
			return *Get(index);
		}

		Type* Data()const
		{
			return begin_;
		}

		Type* Get(const size_t index)const
		{
			if (index >= SIZE)
			{
				return nullptr;
			}
			return begin_ + index;
		}

	private:

		MemoryAllocatorHolder alloc_;

		Type* begin_;

	};
}
