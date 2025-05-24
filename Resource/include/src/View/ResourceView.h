#pragma once
#include "Container.hpp"
#include <utility>

namespace res
{
	struct ResourceID
	{
		uint32_t index = 0;
		uint16_t version = 0;
		uint16_t type = 0;
	};



	template<typename Type>
	class Resource
	{
	public:

		//-------------------------------
		// �쐬
		//-------------------------------

		/// <summary>
		/// ��̃��\�[�X���쐬����R���X�g���N�^
		/// </summary>
		Resource()
			:alloc_(nullptr)
			, count_(0)
			, loader_(nullptr)
			, resource_(nullptr)
		{
		}

		/// <summary>
		/// ���\�[�X���쐬����
		/// </summary>
		/// <param name="alloc"></param>
		/// <param name="loader"></param>
		Resource(utl::IMemoryAllocator* alloc, utl::SharedPtr<IResourceLoaderVoid> loader)
			: alloc_(alloc)
			, count_(0)
			, loader_(loader)
			, resource_(nullptr)
		{

		}

		//-------------------------------
		// �J��
		//-------------------------------

		void Reset()
		{
			alloc_.Reset();
			count_ = 0;
			resource_ = nullptr;
			loader_.Reset();
		}

		~Resource()
		{

		}

		//-------------------------------
		// ���
		//-------------------------------

		Resource(Resource<Type>&& src)noexcept
			: alloc_(src.alloc_.Get())
			, count_(src.count_)
			, loader_(std::move(src.loader_))
			, resource_(src.resource_)
		{
			src.Reset();
		}
		Resource& operator=(Resource<Type>&& src)noexcept
		{
			alloc_.Reset(src.alloc_.Get());
			count_ = src.count_;
			loader_ = std::move(src.loader_);
			resource_ = src.resource_;

			src.Reset();

			return *this;
		}

		//-------------------------------
		// ����
		//-------------------------------

		inline bool Look()
		{
			if (count_ == 0)
			{
				LoadResource();
			}

			++count_;
			return true;
		}

		inline void ReLoad()
		{
			if (!IsActive())
			{
				return;
			}

			ReleaseResource();
			LoadResource();
		}

		inline void UnLook()
		{
			if (!IsActive())
			{
				return;
			}

			--count_;

			if (count_ == 0)
			{
				ReleaseResource();
			}
		}

		inline bool IsActive()const
		{
			return count_ != 0 && resource_ != nullptr;
		}

		//-------------------------------
		// �擾
		//-------------------------------

		Type* Get()const
		{
			return resource_;
		}

	private:

		bool LoadResource()
		{
			Type* ptr = alloc_.Allocate<Type>();
			bool loadResult = loader_->Load(ptr);
			if (!loadResult)
			{
				return false;
			}
			resource_ = ptr;
			return true;
		}

		void ReleaseResource()
		{
			resource_->~Type();
			alloc_.Deallocate(static_cast<void*>(resource_));
			resource_ = nullptr;
		}

	private:

		utl::MemoryAllocatorHolder alloc_;

		size_t count_ = 0;

		utl::SharedPtr<IResourceLoaderVoid> loader_;

		Type* resource_ = nullptr;

	};


	template<typename ResType>
	class ResourceView
	{
	public:

		//--------------------------------------
		// �쐬
		//--------------------------------------

		/// <summary>
		/// ���View���쐬����
		/// </summary>
		ResourceView()
			:res_(nullptr)
		{

		}

		/// <summary>
		/// ���\�[�X�̃r���[���쐬����
		/// </summary>
		/// <param name="res"></param>
		ResourceView(Resource<ResType>& res)
			:res_(&res)
		{
			res.Look();
		}

		//--------------------------------------
		// ���
		//--------------------------------------

		/// <summary>
		/// �R�s�[���č쐬����R���X�g���N�^
		/// </summary>
		/// <param name="src"></param>
		ResourceView(const ResourceView<ResType>& src)
			:res_(src)
		{
			res_->Look();
		}
		/// <summary>
		/// �R�s�[����
		/// </summary>
		/// <param name="src"></param>
		/// <returns></returns>
		ResourceView& operator=(const ResourceView<ResType>& src)
		{
			Reset();

			res_ = src.res_;
			res_->Look();
			return *this;
		}

		/// <summary>
		/// �ق���View���珊�L�����ړ����č쐬����R���X�g���N�^
		/// </summary>
		/// <param name="src"></param>
		ResourceView(ResourceView<ResType>&& src)
			:res_(src.res_)
		{
			src.res_ = nullptr;
		}
		/// <summary>
		/// �ق���View���珊�L�����ړ�����
		/// </summary>
		/// <param name="src"></param>
		/// <returns></returns>
		ResourceView& operator=(ResourceView<ResType>&& src)
		{
			Reset();
			res_ = src.res_;
			src.res_ = nullptr;
		}

		//--------------------------------------
		// �J��
		//--------------------------------------

		void Reset()
		{
			if (res_ != nullptr)
			{
				res_->UnLook();
				res_ = nullptr;
			}
		}

		~ResourceView()
		{
			Reset();
		}

		const ResType* Get()const
		{
			if (res_ == nullptr)
			{
				return nullptr;
			}
			return res_->Get();
		}

	private:

		Resource<ResType>* res_;

	};

}