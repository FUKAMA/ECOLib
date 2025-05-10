#pragma once
#include "Memory.hpp"

namespace utl
{
	template<typename Type>
	class Vector
	{
	public:
		//------------------------------------------
		// �쐬
		//------------------------------------------

		Vector(IMemoryAllocator* alloc = nullptr)
			: alloc_(alloc)
			, begin_(nullptr)
			, size_(0)
			, capacity_(0)
		{

		}

		Vector(const size_t capacity, IMemoryAllocator* alloc = nullptr)
			: alloc_(alloc)
			, begin_(nullptr)
			, size_(0)
			, capacity_(0)
		{
			Reserve(capacity);
		}

		//------------------------------------------
		// ���
		//------------------------------------------

		/// <summary>
		/// 
		/// </summary>
		/// <param name="src"></param>
		Vector(const Vector& src)
			: alloc_(src.alloc_.Get())
			, begin_(nullptr)
			, size_(0)
			, capacity_(0)
		{
			*this = src;
		}
		/// <summary>
		/// 
		/// </summary>
		/// <param name="src"></param>
		/// <returns></returns>
		Vector& operator=(const Vector& src)
		{
			Clear();

			alloc_.Reset(src.alloc_.Get());
			size_ = src.size_;
			capacity_ = src.capacity_;

			// �V�������������m��
			Type* newBegin = alloc_.Allocate<Type>(capacity_);
			assert(newBegin != nullptr, "���������m�ۂł��܂���ł���");


			for (size_t i = 0; i < size_; i++)
			{
				new (newBegin + i) Type(*(src.begin_ + i));
			}
			return *this;
		}


		/// <summary>
		/// Src�������č쐬����
		/// </summary>
		/// <param name="src"></param>
		Vector(Vector&& src)
			: alloc_(src.alloc_.Get())
			, begin_(src.begin_)
			, size_(src.size_)
			, capacity_(src.capacity_)
		{
			src.alloc_.Reset();
			src.begin_ = nullptr;
			src.size_ = 0;
			src.capacity_ = 0;
		}
		/// <summary>
		/// Src��������
		/// </summary>
		/// <param name="src"></param>
		/// <returns></returns>
		Vector& operator =(Vector&& src)
		{
			Clear();

			alloc_.Reset(src.alloc_.Get());
			begin_ = src.begin_;
			size_ = src.size_;
			capacity_ = src.capacity_;

			src.alloc_.Reset();
			src.begin_ = nullptr;
			src.size_ = 0;
			src.capacity_ = 0;

			return *this;
		}

		//------------------------------------------
		// �J��
		//------------------------------------------

		~Vector()
		{
			Clear();
		}

		void Clear()
		{
			if (capacity_ <= 0)
			{
				return;
			}

			for (size_t i = 0; i < size_; ++i)
			{
				Type* ptr = begin_ + i;
				ptr->~Type();
			}
			alloc_.Deallocate(begin_);
			begin_ = nullptr;
			size_ = 0;
			capacity_ = 0;
		}

		//------------------------------------------
		// �ǉ�
		//------------------------------------------

		Type* PushBack(const Type& src)
		{
			// �e�ʂ�����Ȃ���Ίm�ۂ���
			if (capacity_ <= size_)
			{
				Reserve(capacity_ * 2);
			}

			const size_t insertIndex = size_++;

			Type* ptr = begin_ + insertIndex;

			new (ptr) Type(src);

			return ptr;
		}

		template<typename...ArgTypes>
		Type* EmplaceBack(ArgTypes&&...args)
		{
			// �e�ʂ�����Ȃ���Ίm�ۂ���
			if (capacity_ <= size_)
			{
				Reserve(capacity_ * 2);
			}

			const size_t insertIndex = size_++;

			Type* ptr = begin_ + insertIndex;

			new (ptr) Type(args...);

			return ptr;
		}

		//------------------------------------------
		// �擾
		//------------------------------------------

		Type* Get(const size_t index)const
		{
			assert(index <= size_ && "�͈͊O�A�N�Z�X�ł�");

			return begin_ + index;
		}

		Type& operator[](const size_t index)const
		{
			return *Get(index);
		}

		Type* Begin()const
		{
			return begin_;
		}

		Type* Back()const
		{
			if (size_ == 0)
			{
				return nullptr;
			}
			return begin_ + (size_ - 1);
		}

		size_t Size()const
		{
			return size_;
		}

		size_t Capacity()const
		{
			return capacity_;
		}

		bool Empty()const
		{
			return size_ == 0;
		}

		//------------------------------------------
		// ����������
		//------------------------------------------

		template<typename...ArgTypes>
		void Resize(const size_t size, ArgTypes&&...args)
		{
			// ����������������Ȃ���Ίm��
			if (size > capacity_)
			{
				Reserve(size);
			}

			// �v�f�����������Ȃ炻�̕���������
			for (size_t i = size_; i < size; i++)
			{
				Type* ptr = begin_ + i;
				new (ptr)Type(args...);
			}
		}

		/// <summary>
		/// �L���p�V�e�B���g��
		/// �k���͂��Ȃ�
		/// </summary>
		/// <param name="capacity"></param>
		void Reserve(size_t capacity)
		{
			if (capacity == 0)
			{
				capacity = 1;
			}
			// �w�肵���e�ʂ����Ɠ���������ȉ��������牽�����Ȃ�
			if (capacity_ >= capacity)
			{
				return;
			}

			ChangeCapacity(capacity);
		}

		/// <summary>
		/// �L���p�V�e�B���k��
		/// �g��͂��Ȃ�
		/// �T�C�Y��菬�����͂Ȃ�Ȃ�
		/// </summary>
		/// <param name="capacity"></param>
		void Reduce(const size_t capacity)
		{
			// �w�肵���e�ʂ����Ɠ���������ȉ��������牽�����Ȃ�
			if (capacity_ <= capacity)
			{
				return;
			}
			ChangeCapacity(capacity);
		}

		/// <summary>
		/// �L���p�V�e�B��ύX
		/// �T�C�Y��菬�����͂Ȃ�Ȃ�
		/// </summary>
		/// <param name="capacity"></param>
		void ChangeCapacity(const size_t capacity)
		{
			const bool equalCapacity = capacity == capacity_;
			const bool smallerSize = size_ > capacity;

			if (equalCapacity || smallerSize)
			{
				return;
			}

			ChangeMemorySize(capacity);

		}

		/// <summary>
		/// �������̃T�C�Y��ύX���A�Ĕz�u���s��
		/// �e�ʂ��v�f����菭�Ȃ��ꍇ�A�v�f�͊J�������
		/// </summary>
		/// <param name="capacity"></param>
		void ChangeMemorySize(const size_t capacity)
		{
			if (capacity == capacity_)
			{
				return;
			}

			// �V�������������m��
			Type* newBegin = alloc_.Allocate<Type>(capacity);
			assert(newBegin != nullptr, "���������m�ۂł��܂���ł���");
			if (newBegin == nullptr)
			{
				return;
			}

			size_t afterSize = size_;
			if (size_ > capacity)
			{
				afterSize = capacity;
			}

			// �V�����������ɗv�f���R�s�[
			for (size_t i = 0; i < afterSize; ++i)
			{
				new (newBegin + i) Type(Move(*(begin_ + i)));
			}

			// �O�̃������̃C���X�^���X���J��
			for (size_t i = 0; i < afterSize; ++i)
			{
				Type* ptr = begin_ + i;
				ptr->~Type();
			}

			// �O�̃��������J�����A�V�������������Z�b�g
			alloc_.Deallocate(begin_);
			begin_ = newBegin;
			size_ = afterSize;
			capacity_ = capacity;

		}




	private:

		MemoryAllocatorHolder alloc_;

		Type* begin_;

		size_t size_;

		size_t capacity_;

	};
}
