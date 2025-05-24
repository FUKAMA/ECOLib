#pragma once
#include <cassert>
#include "../../Allocator/MemoryAllocator.h"

namespace utl
{

	struct RefCounter
	{
		RefCounter() = default;

		RefCounter(void* ptr, size_t size, IMemoryAllocator* alloc)
			: ptr_(ptr)
			, alloc_(alloc)
			, size_(size)
			, counter_(0)
		{

		}

		void* ptr_;

		size_t size_;

		MemoryAllocatorHolder alloc_;

		int counter_;
	};
	template<typename Type>
	struct RefCounterToIns
	{
	public:
		RefCounterToIns() = delete;

		Type ins;
		RefCounter typeCounter;
	};

	template<typename Type, typename...ArgTypes>
	inline RefCounter* MakeRefCounterWithAlloc(IMemoryAllocator* alloc, ArgTypes&&...args)
	{
		MemoryAllocatorHolder allocHolder(alloc);

		// RefCounterToIns���g���ARefCounter��Type�̃������𓯎��Ɋm��
		void* mem = allocHolder.Allocate(sizeof(RefCounterToIns<Type>));
		RefCounterToIns<Type>* allocResult = static_cast<RefCounterToIns<Type>*>(mem);
		// �m�ۂ����������ɃC���X�^���X���쐬
		new (&allocResult->ins) Type(args...);
		new (&allocResult->typeCounter) RefCounter(&allocResult->ins, sizeof(Type), alloc);

		return &allocResult->typeCounter;
	}
	/// <summary>
	/// ���L�|�C���^
	/// </summary>
	/// <typeparam name="Type"></typeparam>
	template<typename Type>
	class SharedPtr
	{
	private:
		template<typename SrcType>
		friend class SharedPtr;




	private:

		//---------------------------------
		// �쐬
		//---------------------------------

		template<typename SrcType, typename...ArgTypes>
		friend SharedPtr<SrcType> MakeSharedWithAlloc(IMemoryAllocator* alloc, ArgTypes&&...args);

		/// <summary>
		/// ���\�[�X�̃J�E���^��n���č쐬����R���X�g���N�^
		/// </summary>
		/// <param name="ref"></param>
		SharedPtr(RefCounter* ref)
			:ref_(ref)
		{
			++ref->counter_;
		}

	public:

		/// <summary>
		/// ��̋��L�|�C���^���쐬����R���X�g���N�^
		/// </summary>
		SharedPtr()
			:ref_(nullptr)
		{

		}


		//---------------------------------
		// ���
		//---------------------------------



		/// <summary>
		/// ���L�|�C���^���R�s�[���č쐬����R���X�g���N�^
		/// ���L�J�E���g�͑�����
		/// </summary>
		/// <param name="src"></param>
		SharedPtr(const SharedPtr& src)
			:ref_(src.ref_)
		{
			if (ref_ == nullptr)
			{
				return;
			}

			++ref_->counter_;
		}
		/// <summary>
		/// ���L�|�C���^���R�s�[����
		/// ���L�J�E���g�͑�����
		/// ���łɊǗ����Ă���Q�Ƃ��O��
		/// </summary>
		/// <param name="src"></param>
		/// <returns></returns>
		SharedPtr<Type>& operator=(const SharedPtr& src)
		{
			// �������g�������ĂȂ����m�F
			if (this == &src || this->ref_ == src.ref_)
			{
				return *this;
			}

			// ���łɈ����Ă郊�\�[�X����������J��
			Reset();

			ref_ = src.ref_;
			if (ref_ == nullptr)
			{
				return *this;
			}

			++ref_->counter_;
			return *this;
		}


		/// <summary>
		/// �h���N���X����L���X�g���ă��[�u
		/// </summary>
		/// <typeparam name="BaseType"></typeparam>
		/// <param name=""></param>
		template<typename SubType, std::enable_if_t<std::is_convertible_v<SubType*, Type*>, int> = 0>
		SharedPtr(SharedPtr<SubType>&& src) noexcept
			:ref_(nullptr)
		{
			ref_ = src.ref_;
			src.ref_ = nullptr;
		}

		template<typename SubType, std::enable_if_t<std::is_convertible_v<SubType*, Type*>, int> = 0>
		SharedPtr& operator=(SharedPtr<SubType>&& src)noexcept
		{
			// ���łɈ����Ă郊�\�[�X����������J��
			Reset();

			ref_ = src.ref_;
			src.ref_ = nullptr;
			return *this;
		}


		SharedPtr(nullptr_t)noexcept
			:ref_(nullptr)
		{

		}
		SharedPtr& operator=(nullptr_t)noexcept
		{
			Reset();
			return *this;
		}
		//---------------------------------
		// �J��
		//---------------------------------

		/// <summary>
		/// �Q�Ƃ���߂�
		/// �C���X�^���X���Q�Ƃ��Ă鑶�݂����Ȃ��Ȃ�ΊJ������
		/// </summary>
		void Reset()
		{
			if (ref_ == nullptr)
			{
				return;
			}

			--ref_->counter_;

			// �J�E���^���O�ȉ��ɂȂ�����C���X�^���X���J������
			if (ref_->counter_ <= 0)
			{
				// Ref���Ԃ������A���P�[�^�̓����������u���b�N�ɐ���������Ă��邽�߂���ł܂Ƃ߂ĊJ��
				ref_->alloc_.Deallocate(ref_->ptr_);
			}

			ref_ = nullptr;
		}

		~SharedPtr()
		{
			Reset();
		}

		//---------------------------------
		// �擾
		//---------------------------------

		/// <summary>
		/// �Ǘ����Ă�C���X�^���X�̃|�C���^��Ԃ�
		/// </summary>
		/// <returns></returns>
		Type* Get()const
		{
			if (ref_ == nullptr)
			{
				return nullptr;
			}
			return static_cast<Type*>(ref_->ptr_);
		}

		/// <summary>
		/// �Ǘ����Ă�C���X�^���X�̃|�C���^�ɃA�N�Z�X����
		/// </summary>
		/// <returns></returns>
		Type* operator->()const
		{
			assert(ref_ != nullptr && "nullptr�̎��̂ɃA�N�Z�X���悤�Ƃ��Ă��܂�");
			assert(ref_->ptr_ != nullptr && "nullptr�̎��̂ɃA�N�Z�X���悤�Ƃ��Ă��܂�");
			return Get();
		}

		/// <summary>
		/// �Ǘ����Ă���C���X�^���X�̎��̂�Ԃ�
		/// </summary>
		/// <returns></returns>
		Type& operator*()const
		{
			assert(ref_ != nullptr && "nullptr�̎��̂ɃA�N�Z�X���悤�Ƃ��Ă��܂�");
			return *Get();
		}

		/// <summary>
		/// �L���ȃC���X�^���X���Ǘ����Ă��邩
		/// </summary>
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

		/// <summary>
		/// ���|�C���^�ƊǗ����Ă���C���X�^���X�̃|�C���^����v�����true
		/// </summary>
		/// <param name="ptr"></param>
		/// <returns></returns>
		const bool operator==(const Type* ptr)const
		{
			if (ref_ == nullptr)
			{
				return false;
			}

			return ref_->GetPtr() == ptr;
		}
		/// <summary>
		/// SharedPtr�������C���X�^���X���Ǘ����Ă����true
		/// </summary>
		/// <param name="ptr"></param>
		/// <returns></returns>
		const bool operator==(const SharedPtr<Type>& ptr)const
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

		/// <summary>
		/// ���|�C���^�ƊǗ����Ă���C���X�^���X�̃|�C���^����v���Ȃ����true
		/// </summary>
		/// <param name="ptr"></param>
		/// <returns></returns>
		const bool operator!=(const Type* ptr)const
		{
			return !((*this) == ptr);
		}
		/// <summary>
		/// SharedPtr�������C���X�^���X���Ǘ����Ă��Ȃ����true
		/// </summary>
		/// <param name="ptr"></param>
		/// <returns></returns>
		const bool operator!=(const SharedPtr<Type>& ptr)const
		{
			return !((*this) == ptr);
		}

	private:

		RefCounter* ref_;

	};


	/// <summary>
	/// �A���P�[�^���w�肵��SharedPtr���쐬����
	/// </summary>
	/// <typeparam name="Type"></typeparam>
	/// <typeparam name="...ArgTypes"></typeparam>
	/// <param name="alloc"></param>
	/// <param name="...args"></param>
	/// <returns></returns>
	template<typename Type, typename...ArgTypes>
	inline SharedPtr<Type> MakeSharedWithAlloc(IMemoryAllocator* alloc, ArgTypes&&...args)
	{
		RefCounter* counter = MakeRefCounterWithAlloc<Type>(alloc, args...);
		// View�����
		return SharedPtr<Type>(counter);
	}

	/// <summary>
	/// SharedPtr���쐬����
	/// </summary>
	/// <typeparam name="Type"></typeparam>
	/// <typeparam name="...ArgTypes"></typeparam>
	/// <param name="...args"></param>
	/// <returns></returns>
	template<typename Type, typename...ArgTypes>
	inline SharedPtr<Type> MakeShared(ArgTypes&&...args)
	{
		return MakeSharedWithAlloc<Type>(nullptr, args...);
	}
}
