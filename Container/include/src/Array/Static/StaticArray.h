#pragma once
#include "Memory.hpp"

namespace utl
{
	template<typename Type, size_t SIZE>
	class StaticArray
	{
	public:

		//--------------------------------------
		// �쐬
		//--------------------------------------

		StaticArray()
			:mem_()
		{
			for (size_t i = 0; i < SIZE; ++i)
			{
				new (Get(i)) Type();
			}
		}

		template<typename...ArgTypes>
		StaticArray(ArgTypes&&...args)
			:mem_()
		{
			for (size_t i = 0; i < SIZE; ++i)
			{
				new (Get(i)) Type(args...);
			}
		}

		//--------------------------------------
		// �J��
		//--------------------------------------

		~StaticArray()
		{
			for (size_t i = 0; i < SIZE; ++i)
			{
				Get(i)->~Type();
			}
		}

		//--------------------------------------
		// �擾
		//--------------------------------------

		Type& operator[](const size_t index)const
		{
			assert(index < SIZE && "�͈͊O�A�N�Z�X�ł�");
			return *Get(index);
		}

		Type* Data()const
		{
			return Get(0);
		}

		constexpr size_t Size()const
		{
			return SIZE;
		}

		void* GetVoid(const size_t index)
		{
			assert(index < SIZE && "�͈͊O�A�N�Z�X�ł�");

			char* ptr = &mem_[index * sizeof(Type)];
			return static_cast<void*>(ptr);
		}

		Type* Get(const size_t index)
		{
			void* ptr = GetVoid(index);
			return static_cast<Type*>(ptr);
		}


	private:
		
		alignas(Type) char mem_[SIZE * sizeof(Type)];

	};
}