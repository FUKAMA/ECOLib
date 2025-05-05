#pragma once
#include "Memory.hpp"

namespace utl
{
	/// <summary>
	/// �v�f���⃁�����m�ۂ̃^�C�~���O�Ȃǂ𓮓I�Ɏw�肵�č쐬����Œ蒷�̔z��
	/// </summary>
	/// <typeparam name="Type"></typeparam>
	template<typename Type>
	class ResizableArray
	{
	public:

		//--------------------------------------
		// �쐬
		//--------------------------------------

		/// <summary>
		/// ���DynamicArray���쐬����
		/// </summary>
		ResizableArray()
			: alloc_(nullptr)
			, begin_(nullptr)
			, size_(0)
		{

		}

		template<typename...ArgTypes>
		void Create(const size_t size, IMemoryAllocator* alloc = nullptr, ArgTypes&&...args)
		{
			Release();

			alloc_.Reset(alloc);
			begin_ = static_cast<Type*>(alloc_.Allocate(size * sizeof(Type)));
			size_ = size;

			for (size_t i = 0; i < size; ++i)
			{
				Type* ptr = begin_ + i;
				new (ptr) Type(args...);
			}
		}

		template<typename...ArgTypes>
		ResizableArray(const size_t size, IMemoryAllocator* alloc = nullptr, ArgTypes&&...args)
			: alloc_(nullptr)
			, begin_(nullptr)
			, size_(0)
		{
			Create(size, alloc, args...);
		}

		//--------------------------------------
		// �J��
		//--------------------------------------

		void Release()
		{
			if (begin_ == nullptr)
			{
				return;
			}

			for (size_t i = 0; i < size_; ++i)
			{
				Get(i)->~Type();
			}

			alloc_.Deallocate(begin_);
			begin_ = nullptr;
		}


		~ResizableArray()
		{
			Release();
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

		const size_t Size()const
		{
			return size_;
		}

		Type* Get(const size_t index)const
		{
			assert(index < size_ && "�͈͊O�A�N�Z�X�ł�");

			return begin_ + index;
		}

	private:

		MemoryAllocatorHolder alloc_;

		Type* begin_;

		size_t size_;

	};
}

