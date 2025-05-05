#pragma once
#include <cassert>
#include "../../Allocator/MemoryAllocator.h"

namespace utl
{
	template<typename Type>
	class SharedPtr;

	/// <summary>
	/// �Q�ƃJ�E���^�ƃI�u�W�F�N�g�̃|�C���^���Ǘ�����N���X
	/// �Ǘ�����I�u�W�F�N�g�Ƃ��̃C���X�^���X���i�[���郁�����͈����œn��MemoryAllocator�ō쐬����
	/// ���̃N���X�̃C���X�^���X���J������Ƃ��́A�R���X�g���N�^�ɓn�����������A���P�[�^�ŊJ������K�v������
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

		/// <summary>
		/// �L����RefCounter���쐬����R���X�g���N�^
		/// MakeRefCounterWithAlloc����쐬����邱�Ƃ�z�肵�Ă���B
		/// </summary>
		/// <param name="ptr"></param>
		/// <param name="alloc"></param>
		RefCounter(Type* ptr, IMemoryAllocator* alloc)
			:ptr_(ptr)
			, alloc_(alloc)
			, counter_(0)
		{

		}

	public:

		/// <summary>
		/// ���RefCounter���쐬����R���X�g���N�^
		/// </summary>
		RefCounter()
			:ptr_(nullptr)
			, alloc_(nullptr)
			, counter_(0)
		{

		}

		//---------------------------------
		// �J��
		//---------------------------------

		~RefCounter() = default;

		//---------------------------------
		// �J�E���g
		//---------------------------------

		/// <summary>
		/// �Q�ƃJ�E���^�𑝉�
		/// </summary>
		void Look()
		{
			++counter_;
		}

		/// <summary>
		/// �Q�ƃJ�E���^������
		/// �������̊J���͊O���ɔC����
		/// </summary>
		void UnLook()
		{
			--counter_;
		}

		/// <summary>
		/// ���ݎQ�Ƃ��Ă��鐔��Ԃ�
		/// </summary>
		/// <returns></returns>
		int Counter()const
		{
			return counter_;
		}

		/// <summary>
		/// �Ǘ����Ă�C���X�^���X�̃|�C���^���擾
		/// </summary>
		/// <returns></returns>
		Type* GetPtr()const
		{
			return ptr_;
		}

		/// <summary>
		/// ���̃N���X�ƊǗ����Ă�C���X�^���X�̃��������Ǘ�����A���P�[�^�̃|�C���^���擾
		/// </summary>
		/// <returns></returns>
		MemoryAllocatorHolder& GetAlloc()
		{
			return alloc_;
		}

		/// <summary>
		/// �m�ۂ����C���X�^���X���J������
		/// </summary>
		bool Reset()
		{
			if (counter_ > 0 || ptr_ == nullptr)
			{
				return false;
			}

			ptr_->~Type();
			alloc_.Deallocate(ptr_);
			ptr_ = nullptr;
			counter_ = 0;

			return true;
		}

	private:

		Type* ptr_;

		MemoryAllocatorHolder alloc_;

		int counter_;

	};

	/// <summary>
	/// �Q�ƃJ�E���^�ƃC���X�^���X�̃���������ׂĔz�u���邽�߂̍\����
	/// �|�C���^�Ƃ��Ă݂̗̂��p��ړI�Ƃ��A�C���X�^���X���͋����Ȃ�
	/// Type�̃C���X�^���X���J�������痼���J�������悤��Ins��擪�ɔz�u
	/// </summary>
	/// <typeparam name="Type"></typeparam>
	template<typename Type>
	class RefCounterToIns
	{
	public:
		RefCounterToIns() = delete;

		Type ins;
		RefCounter<Type> typeCounter;
	};

	/// <summary>
	/// �������A���P�[�^���g����RefCounter���쐬����֐�
	/// RefCounter��RefCounter���Ǘ�����C���X�^���X�̗������������A���P�[�^�Ŋm�ۂ����������ɔz�u����
	/// </summary>
	/// <typeparam name="Type"></typeparam>
	/// <typeparam name="...ArgTypes"></typeparam>
	/// <param name="alloc"></param>
	/// <param name="...args"></param>
	/// <returns></returns>
	template<typename Type, typename...ArgTypes>
	inline RefCounter<Type>* MakeRefCounterWithAlloc(IMemoryAllocator* alloc, ArgTypes&&...args)
	{
		MemoryAllocatorHolder allocHolder(alloc);

		// RefCounterToIns���g���ARefCounter��Type�̃������𓯎��Ɋm��
		void* mem = allocHolder.Allocate(sizeof(RefCounterToIns<Type>));
		RefCounterToIns<Type>* allocResult = static_cast<RefCounterToIns<Type>*>(mem);
		// �m�ۂ����������ɃC���X�^���X���쐬
		new (&allocResult->ins) Type(args...);
		new (&allocResult->typeCounter) RefCounter<Type>(&allocResult->ins, alloc);

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

		//---------------------------------
		// �쐬
		//---------------------------------

		template<typename T, typename...ArgTypes>
		friend SharedPtr<T> MakeSharedWithAlloc(IMemoryAllocator* alloc, ArgTypes&&...args);

		/// <summary>
		/// ���\�[�X�̃J�E���^��n���č쐬����R���X�g���N�^
		/// </summary>
		/// <param name="ref"></param>
		SharedPtr(RefCounter<Type>* ref)
			:ref_(ref)
		{
			ref->Look();
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

			ref_->Look();
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

			ref_->Look();
			return *this;
		}

		/// <summary>
		/// ���L�|�C���^�̏��L�����ړ����č쐬����R���X�g���N�^
		/// </summary>
		/// <param name="src"></param>
		SharedPtr(SharedPtr<Type>&& src)noexcept
			:ref_(src.ref_)
		{
			src.ref_ = nullptr;
		}
		/// <summary>
		/// ���L�|�C���^�̏��L�����ړ�����
		/// ���łɊǗ����Ă���Q�Ƃ��O��
		/// </summary>
		/// <param name="src"></param>
		/// <returns></returns>
		SharedPtr<Type>& operator=(SharedPtr<Type>&& src)noexcept
		{
			// ���łɈ����Ă郊�\�[�X����������J��
			Reset();

			ref_ = src.ref_;
			src.ref_ = nullptr;
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

			ref_->UnLook();

			// �J�E���^���O�ȉ��ɂȂ�����C���X�^���X���J������
			if (ref_->Counter() <= 0)
			{
				ref_->Reset();
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
			return ref_->GetPtr();
		}

		/// <summary>
		/// �Ǘ����Ă�C���X�^���X�̃|�C���^�ɃA�N�Z�X����
		/// </summary>
		/// <returns></returns>
		Type* operator->()const
		{
			assert(ref_ != nullptr && "nullptr�̎��̂ɃA�N�Z�X���悤�Ƃ��Ă��܂�");
			assert(ref_->GetPtr() != nullptr && "nullptr�̎��̂ɃA�N�Z�X���悤�Ƃ��Ă��܂�");
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

		RefCounter<Type>* ref_;

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
		RefCounter<Type>* counter = MakeRefCounterWithAlloc<Type>(alloc, args...);
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
