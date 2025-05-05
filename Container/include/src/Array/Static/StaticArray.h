#pragma once
#include "Memory.hpp"

namespace utl
{
	template<typename Type, size_t SIZE>
	class StaticArray
	{
	public:

		//--------------------------------------
		// çÏê¨
		//--------------------------------------


		StaticArray(IMemoryAllocator* alloc = nullptr)
			: alloc_(alloc)
			, begin_(alloc_.Allocate(SIZE * sizeof(Type)))
		{
			for (size_t i = 0; i < SIZE; ++i)
			{
				Type* ptr = begin_ + i;
				new (ptr) Type();
			}
		}

		template<typename...ArgTypes>
		StaticArray(IMemoryAllocator* alloc, ArgTypes&&...args)
			: alloc_(alloc)
			, begin_(alloc_.Allocate(SIZE * sizeof(Type)))
		{
			for (size_t i = 0; i < SIZE; ++i)
			{
				Type* ptr = begin_ + i;
				new (ptr) Type(args...);
			}
		}

		template<typename...ArgTypes>
		StaticArray(ArgTypes&&...args)
			: StaticArray::StaticArray(nullptr, args...)
		{
		}
		//template<typename...ArgTypes>
		//StaticArray(ArgTypes&&...args)
		//	: alloc_(nullptr)
		//	, begin_(alloc_.Allocate(SIZE * sizeof(Type)))
		//{
		//	for (size_t i = 0; i < SIZE; ++i)
		//	{
		//		Type* ptr = begin_ + i;
		//		new (ptr) Type(args...);
		//	}
		//}

		~StaticArray()
		{
			for (size_t i = 0; i < SIZE; ++i)
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
