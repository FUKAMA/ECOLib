#pragma once
#include <cassert>
#include "Memory.hpp"

namespace utl
{

	/// <summary>
	/// �v�f����ύX���邱�Ƃ��ł���z��
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
			Reset();

			alloc_.Reset(alloc);
			begin_ = alloc_.Allocate<Type>(size);
			size_ = size;

			for (size_t i = 0; i < size; ++i)
			{
				Type* ptr = begin_ + i;
				new (ptr) Type(Move(args...));
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
		// ���
		//--------------------------------------

		ResizableArray(const ResizableArray& src)
			: alloc_(src.alloc_.Get())
			, begin_(src.alloc_.Allocate<Type>(src.Size()))
			, size_(src.size_)
		{
			MoveMem(begin_, src.Data(), src.Size());
		}

		ResizableArray& operator=(const ResizableArray& src)
		{
			Reset();

			alloc_.Reset(src.alloc_.Get());
			begin_ = alloc_.Allocate<Type>(src.Size());
			size_ = src.Size();

			MoveMem(begin_, src.Data(), src.Size());

			return *this;
		}

		ResizableArray(ResizableArray&& src)noexcept
			: alloc_(src.alloc_.Get())
			, begin_(src.begin_)
			, size_(src.size_)
		{
			src.alloc_.Reset();
			src.begin_ = nullptr;
			src.size_ = 0;
		}

		ResizableArray& operator=(ResizableArray&& src)noexcept
		{
			Reset();

			alloc_.Reset(src.alloc_.Get());
			begin_ = src.begin_;
			size_ = src.Size();

			src.alloc_.Reset();
			src.begin_ = nullptr;
			src.size_ = 0;
			return *this;
		}

		//--------------------------------------
		// �J��
		//--------------------------------------

		/// <summary>
		/// 
		/// </summary>
		void Reset()
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
			size_ = 0;
		}


		~ResizableArray()
		{
			Reset();
		}

		//--------------------------------------
		// ����
		//--------------------------------------

		template<typename...ArgTypes>
		bool Resize(const size_t afterSize, ArgTypes&&...args)
		{
			const size_t beforeSize = size_;
			// ���ƃT�C�Y���ς��Ȃ���Ή������Ȃ�
			if (beforeSize == afterSize)
			{
				return false;
			}

			// �ύX��v�f���Ȃ��Ȃ�Ȃ烊�Z�b�g
			if (afterSize == 0)
			{
				Reset();
				return true;
			}



			// �V�������������m��
			Type* newMem = alloc_.Allocate<Type>(afterSize);

			// �ړ����郁����������Ȃ�ړ�
			size_t moveNum = afterSize;
			if (moveNum > beforeSize)
			{
				moveNum = beforeSize;
			}
			if (moveNum > 0)
			{
				MoveMem(newMem, begin_, moveNum);
			}

			// ���g���Ă郁�����ɔz�u���ꂽ�C���X�^���X���J�����A�V�������������Z�b�g
			for (size_t i = 0; i < beforeSize; ++i)
			{
				Get(i)->~Type();
			}
			alloc_.Deallocate(begin_);
			begin_ = newMem;

			// �v�f���쐬�����Ȃ�R���X�g���N�^���Ă�ŃC���X�^���X��z�u
			if (beforeSize < afterSize)
			{
				for (size_t i = beforeSize; i < afterSize; ++i)
				{
					new (Get(i)) Type(args...);
				}
			}

			size_ = afterSize;

			return true;
		}


		//--------------------------------------
		// �擾
		//--------------------------------------

		inline Type& operator[](const size_t index)const
		{
			return *Get(index);
		}

		inline Type* Data()const
		{
			return begin_;
		}

		inline const size_t Size()const
		{
			return size_;
		}

		inline Type* Get(const size_t index)const
		{
			assert(index < size_ && "�͈͊O�A�N�Z�X�ł�");
			return begin_ + index;
		}

	private:
		void MoveMem(Type* destBegin, Type* srcBegin, size_t size)
		{
			for (size_t i = 0; i < size; ++i)
			{
				Type* ptr = destBegin + i;
				new (ptr) Type(Move(*(srcBegin + i)));
			}
		}
	private:

		MemoryAllocatorHolder alloc_;

		Type* begin_;

		size_t size_;

	};
}

