#pragma once
#include "Memory.hpp"

namespace utl
{
	/// <summary>
	/// �v�f���⃁�����m�ۂ̃^�C�~���O�Ȃǂ𓮓I�Ɏw�肵�č쐬����Œ蒷�̔z��
	/// </summary>
	/// <typeparam name="Type"></typeparam>
	template<typename Type, size_t SIZE>
	class DynamicArray
	{
	public:

		//--------------------------------------
		// �쐬
		//--------------------------------------

		template<typename...ArgTypes>
		DynamicArray(IMemoryAllocator* alloc = nullptr, ArgTypes&&...args)
			: alloc_(alloc)
			, begin_(static_cast<Type*>(alloc_.Allocate(SIZE * sizeof(Type))))
		{
			for (size_t i = 0; i < SIZE; ++i)
			{
				Type* ptr = begin_ + i;
				new (ptr) Type(args...);
			}
		}

		//--------------------------------------
		// �J��
		//--------------------------------------

		~DynamicArray()
		{
			if (begin_ == nullptr)
			{
				return;
			}

			for (size_t i = 0; i < SIZE; ++i)
			{
				Get(i)->~Type();
			}

			alloc_.Deallocate(begin_);
			begin_ = nullptr;
		}

		//--------------------------------------
		// �擾
		//--------------------------------------

		Type& operator[](const size_t index)const
		{
			return *Get(index);
		}

		Type* Data()const
		{
			return begin_;
		}

		constexpr size_t Size()const
		{
			return SIZE;
		}

		Type* Get(const size_t index)const
		{
			assert(index < SIZE && "�͈͊O�A�N�Z�X�ł�");

			return begin_ + index;
		}

	private:

		MemoryAllocatorHolder alloc_;

		Type* begin_;

	};
}