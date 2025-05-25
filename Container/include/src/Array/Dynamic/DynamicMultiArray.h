#pragma once
#include "Memory.hpp"
#include "TypeInfo.hpp"

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

	template<typename Type>
	inline void ReleaseTypeMemories(char* begin, size_t& index, size_t num)
	{
		for (size_t i = 0; i < num; ++i)
		{
			void* voidPtr = static_cast<void*>(begin + index);
			Type* ptr = static_cast<Type*>(voidPtr);
			ptr->~Type();
			index += sizeof(Type);
		}
	}


	template<typename Type>
	struct TypeIDPtr
	{

	};

	template<typename CheckType, typename SubType>
	inline void CheckEqualType(size_t num, size_t& index, size_t& typeIndex)
	{
		if (typeIndex == 0)
		{
			return;
		}
		if (utl::GetTypeID<CheckType>() == utl::GetTypeID<SubType>())
		{
			--typeIndex;
		}
		if (typeIndex == 0)
		{
			return;
		}
		index += sizeof(CheckType) * num;
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


		~DynamicMultiArray()
		{
			size_t index = 0;
			(ReleaseTypeMemories<Types>(static_cast<char*>(begin_), index, size_), ...);
			alloc_.Deallocate(begin_);
		}


		template<typename Type>
		Type* GetBegin(size_t typeIndex = 1)
		{
			size_t beginIndex = 0;
			// 最初に見つかったテンプレート引数と同じ型の先頭アドレスの位置を取得
			(CheckEqualType<Type, Types>(size_, beginIndex, typeIndex), ...);

			char* charPtr = static_cast<char*>(begin_);
			void* resultVoidPtr = static_cast<void*>(charPtr + beginIndex);
			Type* result = static_cast<Type*>(resultVoidPtr);

			return result;
		}






	private:

		MemoryAllocatorHolder alloc_;

		void* begin_;

		size_t size_;


	};
}
