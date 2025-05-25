#pragma once
#include "Memory.hpp"

namespace utl
{

	template<typename Type>
	inline void AddTypeSize(size_t& result)
	{
		result += sizeof(Type);
	}


	template<typename...Types>
	inline size_t TypesSize()
	{
		size_t result = 0;

		(AddTypeSize<Types>(result), ...);

		return result;
	}

	template<typename Type>
	inline void InitializeTypeMemories(char* begin, size_t& index, size_t num)
	{
		for (size_t i = 0; i < num; ++i)
		{
			void* voidPtr = static_cast<void*>(begin + index);
			Type* ptr = static_cast<Type*>(voidPtr);
			new (ptr) Type();
			index += sizeof(Type);
		}
	}
	template<typename Type>
	inline void InitializeTypeMemories(char* begin, size_t& index, size_t num, const Type& src)
	{
		for (size_t i = 0; i < num; ++i)
		{
			void* voidPtr = static_cast<void*>(begin + index);
			Type* ptr = static_cast<Type*>(voidPtr);
			new (ptr) Type(src);
			index += sizeof(Type);
		}
	}

	template<typename Type1, typename Type2>
	inline bool CheckEqualType()
	{
	}

	template<typename...Types>
	class DynamicMultiArray
	{
	public:
		DynamicMultiArray(const size_t size, IMemoryAllocator* alloc = nullptr)
			: alloc_(alloc)
			, begin_(alloc_.Allocate(TypesSize<Types...>()* size))
			, size_(size)
		{
			size_t index = 0;
			(InitializeTypeMemories<Types>(static_cast<char*>(begin_), index, size), ...);
		}

		DynamicMultiArray(const size_t size, IMemoryAllocator* alloc, Types&&...srces)
			: alloc_(alloc)
			, begin_(alloc_.Allocate(TypesSize<Types...>()* size))
			, size_(size)
		{
			size_t index = 0;
			(InitializeTypeMemories<Types>(static_cast<char*>(begin_), index, size, srces), ...);
		}

		template<typename Type>
		Type* GetBegin()
		{

		}






	private:

		MemoryAllocatorHolder alloc_;

		void* begin_;

		size_t size_;


	};
}
