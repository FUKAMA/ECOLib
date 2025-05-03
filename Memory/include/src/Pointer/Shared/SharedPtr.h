#pragma once
#include "../../Allocator/MemoryAllocator.h"

namespace utl
{
	template<typename Type>
	class SharedPtr;


	/// <summary>
	/// �Q�ƃJ�E���^�ƃI�u�W�F�N�g�̃|�C���^���Ǘ�����N���X
	/// �Ǘ�����I�u�W�F�N�g�Ƃ��̃C���X�^���X���i�[���郁�����͈����œn��MemoryAllocator�ō쐬����
	/// </summary>
	/// <typeparam name="Type"></typeparam>
	template<typename Type>
	class RefCounter
	{
	private:
		//---------------------------------
		// �쐬
		//---------------------------------

		template<typename T, typename...ArgTypes>
		friend RefCounter<T>* MakeRefCounterWithAlloc(IMemoryAllocator* alloc, ArgTypes&&...args);

		RefCounter(Type* ptr, IMemoryAllocator* alloc)
			:ptr_(ptr)
			, alloc_(alloc)
			, counter_(0)
		{

		}

	public:

		RefCounter()
			:ptr_(nullptr)
			, alloc_(nullptr)
			, counter_(0)
		{

		}

		//---------------------------------
		// �쐬
		//---------------------------------

		~RefCounter() = default;

		//---------------------------------
		// �J�E���g
		//---------------------------------

		void Look()
		{
			++counter_;
		}

		void UnLook()
		{

			--counter_;
		}

		int Counter()const
		{
			return counter_;
		}

		Type* GetPtr()const
		{
			return ptr_;
		}

		MemoryAllocatorHolder& GetAlloc()
		{
			return alloc_;
		}

		void Reset()
		{
			if (counter_ > 0 || ptr_ == nullptr)
			{
				return;
			}

			ptr_->~Type();
			alloc_.Deallocate(ptr_);
			ptr_ = nullptr;
			counter_ = 0;
		}

	private:

		Type* ptr_;

		MemoryAllocatorHolder alloc_;

		int counter_;

	};

	template<typename Type, typename...ArgTypes>
	RefCounter<Type>* MakeRefCounterWithAlloc(IMemoryAllocator* alloc, ArgTypes&&...args)
	{
		MemoryAllocatorHolder allocHolder(alloc);

		void* mem = allocHolder.Allocate(sizeof(Type));
		new(mem) Type(args...);
		Type* ptr = static_cast<Type*>(mem);

		// Counter�����
		void* counterMem = allocHolder.Allocate(sizeof(RefCounter<Type>));
		new (counterMem) RefCounter<Type>(ptr, alloc);
		RefCounter<Type>* counter = static_cast<RefCounter<Type>*>(counterMem);

		return counter;
	}



	template<typename Type>
	class SharedPtr
	{
	private:

		//---------------------------------
		// �쐬
		//---------------------------------

		template<typename T, typename...ArgTypes>
		friend SharedPtr<T> MakeSharedWithAlloc(IMemoryAllocator* alloc, ArgTypes&&...args);

		SharedPtr(RefCounter<Type>* ref)
			:ref_(ref)
		{
			ref->Look();
		}
	public:

		SharedPtr()
			:ref_(nullptr)
		{

		}


		//---------------------------------
		// ���
		//---------------------------------

		SharedPtr(const SharedPtr& src)
			:ref_(src.ref_)
		{
			if (ref_ == nullptr)
			{
				return;
			}

			ref_->Look();
		}
		SharedPtr<Type>& operator=(const SharedPtr& src)
		{
			// ���łɈ����Ă郊�\�[�X����������J��
			Reset();

			ref_ = src.ref_;
			if (ref_ == nullptr)
			{
				return *this;
			}

			ref_->Look();
			return *this;
		}

		SharedPtr(SharedPtr<Type>&& src)noexcept
			:ref_(src.ref_)
		{
			ref_->Look();
			src.Reset();
		}

		SharedPtr<Type>& operator=(SharedPtr<Type>&& src)
		{
			Reset();
			ref_ = src.ref_;
			src.ref_ = nullptr;
			return *this;
		}


		//---------------------------------
		// �J��
		//---------------------------------

		void Reset()
		{
			if (ref_ == nullptr)
			{
				return;
			}

			ref_->UnLook();
			if (ref_->Counter() > 0)
			{
				return;
			}
			ref_->Reset();
			MemoryAllocatorHolder allocHolder = ref_->GetAlloc();

			allocHolder.Deallocate(ref_);

			ref_ = nullptr;
		}

		~SharedPtr()
		{
			Reset();
		}

		//---------------------------------
		// �擾
		//---------------------------------

		Type* Get()const
		{
			if (ref_ == nullptr)
			{
				return nullptr;
			}
			return ref_->GetPtr();
		}

		Type* operator->()const
		{
			assert(ref_ != nullptr && "nullptr�̎��̂ɃA�N�Z�X���悤�Ƃ��Ă��܂�");
			assert(ref_->GetPtr() != nullptr && "nullptr�̎��̂ɃA�N�Z�X���悤�Ƃ��Ă��܂�");
			return Get();
		}

		Type& operator*()const
		{
			assert(ref_ != nullptr && "nullptr�̎��̂ɃA�N�Z�X���悤�Ƃ��Ă��܂�");
			return *Get();
		}

		operator bool()const
		{
			if (ref_ == nullptr)
			{
				return false;
			}
			return ref_->GetPtr() != nullptr;
		}

		//---------------------------------
		// ��r
		//---------------------------------

		const bool operator==(const Type* ptr)const
		{
			if (ref_ == nullptr)
			{
				return false;
			}

			return ref_->GetPtr() == ptr;
		}
		const bool operator==(const SharedPtr<Type> ptr)const
		{
			if (ref_ == ptr.ref_)
			{
				return true;
			}

			if (ptr.ref_ == nullptr)
			{
				return false;
			}

			return (*this) == ptr.Get();
		}

		const bool operator!=(const Type* ptr)const
		{
			return !((*this) == ptr);
		}
		const bool operator!=(const SharedPtr<Type> ptr)const
		{
			return !((*this) == ptr);
		}

	private:

		RefCounter<Type>* ref_;

	};

	template<typename Type, typename...ArgTypes>
	SharedPtr<Type> MakeSharedWithAlloc(IMemoryAllocator* alloc, ArgTypes&&...args)
	{
		RefCounter<Type>* counter = MakeRefCounterWithAlloc<Type>(alloc, args...);
		// View�����
		return SharedPtr<Type>(counter);
	}

	template<typename Type, typename...ArgTypes>
	SharedPtr<Type> MakeShared(ArgTypes&&...args)
	{
		return MakeSharedWithAlloc<Type>(nullptr, args...);
	}


}
