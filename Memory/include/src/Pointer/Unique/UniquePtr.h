#pragma once
#include "../../Allocator/MemoryAllocator.h"
#include <cstdlib>

namespace utl
{
	/// <summary>
	/// �����Ń��������J������|�C���^
	/// </summary>
	/// <typeparam name="Type"></typeparam>
	template<typename Type>
	class UniquePtr
	{
	private:

		template<typename T, typename...ArgTypes>
		friend UniquePtr<T> MakeUniqueWithAlloc(IMemoryAllocator* allocator, ArgTypes&&...args);

		//----------------------------------------
		// �쐬
		//----------------------------------------

		/// <summary>
		/// �쐬�����C���X�^���X�ƃC���X�^���X���쐬�����������A���P�[�^���w�肵�č쐬����R���X�g���N�^
		/// MakeUnique����̂ݎg�p���邱�Ƃ�z�肵�Ă���
		/// </summary>
		/// <param name="ptr"></param>
		/// <param name="allocator"></param>
		UniquePtr(Type* ptr, IMemoryAllocator* allocator)
			: alloc_(allocator)
			, ptr_(ptr)
		{

		}

	public:

		/// <summary>
		/// ���UniquePtr���쐬����R���X�g���N�^
		/// </summary>
		UniquePtr()
			:ptr_(nullptr)
			, alloc_(nullptr)
		{

		}

		//----------------------------------------
		// ���
		//----------------------------------------

		/// <summary>
		/// �R�s�[�͋֎~
		/// </summary>
		/// <param name=""></param>
		UniquePtr(const UniquePtr&) = delete;
		/// <summary>
		/// �R�s�[�͋֎~
		/// </summary>
		/// <param name=""></param>
		/// <returns></returns>
		UniquePtr& operator=(const UniquePtr&) = delete;

		/// <summary>
		/// ���[�u���s��
		/// </summary>
		/// <param name="src"></param>
		UniquePtr(UniquePtr&& src)noexcept
			:alloc_(src.alloc_), ptr_(src.ptr_)
		{
			src.alloc_.Reset();
			src.ptr_ = nullptr;
		}

		/// <summary>
		/// ���[�u���s��
		/// </summary>
		/// <param name="src"></param>
		/// <returns></returns>
		UniquePtr& operator=(UniquePtr&& src)noexcept
		{
			// ���łɊm�ۂ��Ă郊�\�[�X����������J��
			Reset();

			alloc_ = src.alloc_;
			ptr_ = src.ptr_;

			src.alloc_.Reset();
			src.ptr_ = nullptr;

			return *this;
		}


		//----------------------------------------
		// �J��
		//----------------------------------------

		/// <summary>
		/// �C���X�^���X���J������
		/// </summary>
		void Reset()
		{
			if (!IsActive())
			{
				return;
			}

			ptr_->~Type();

			void* mem = static_cast<void*>(ptr_);
			alloc_.Deallocate(mem);

			alloc_.Reset();
			ptr_ = nullptr;
		}

		~UniquePtr()
		{
			Reset();
		}



		//----------------------------------------
		// �擾
		//----------------------------------------

		/// <summary>
		/// �Ǘ����Ă�C���X�^���X�̃|�C���^���擾
		/// </summary>
		/// <returns></returns>
		inline Type* Get()const
		{
			return ptr_;
		}

		Type* operator->()const
		{
			assert(ptr_ != nullptr && "nullptr��->���g�p���Ă��܂��I");
			return ptr_;
		}

		/// <summary>
		/// �Ǘ����Ă�C���X�^���X�̎��ۂ��Q��
		/// </summary>
		/// <returns></returns>
		Type& operator*()const
		{
			assert(ptr_ != nullptr && "nullptr�̎��̂ɃA�N�Z�X���悤�Ƃ��Ă��܂��I");
			return *ptr_;
		}

		bool IsActive()const
		{
			return ptr_ != nullptr;
		}

		//----------------------------------------
		// ��r
		//----------------------------------------

		const bool operator==(const Type* ptr)const
		{
			return ptr_ == ptr;
		}

		const bool operator!=(const Type* ptr)const
		{
			return !((*this) == ptr);
		}




	private:

		MemoryAllocatorHolder alloc_;

		Type* ptr_;

	};

	/// <summary>
	/// UniquePtr���쐬����֐�
	/// </summary>
	/// <typeparam name="Type"></typeparam>
	/// <typeparam name="...ArgTypes"></typeparam>
	/// <param name="...args"></param>
	/// <param name="allocator"></param>
	/// <returns></returns>
	template<typename Type, typename...ArgTypes>
	inline UniquePtr<Type> MakeUniqueWithAlloc(IMemoryAllocator* allocator, ArgTypes&&...args)
	{
		size_t typeSize = sizeof(Type);
		MemoryAllocatorHolder allocHolder(allocator);

		// ���������m�ۂ��ăC���X�^���X��z�u����
		void* mem = nullptr;
		mem = allocHolder.Allocate(typeSize);
		new (mem) Type(args...);

		// uniquePtr���쐬����
		return UniquePtr<Type>(static_cast<Type*>(mem), allocator);
	}

	template<typename Type, typename...ArgTypes>
	inline UniquePtr<Type> MakeUnique(ArgTypes&&...args)
	{
		return MakeUniqueWithAlloc<Type>(nullptr, args...);
	}
}
