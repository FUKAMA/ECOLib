#pragma once
#include "../../Allocator/MemoryAllocator.h"
#include <cstdlib>

namespace utl
{
	/// <summary>
	/// 自動でメモリを開放するポインタ
	/// </summary>
	/// <typeparam name="Type"></typeparam>
	template<typename Type>
	class UniquePtr
	{
	private:

		template<typename T, typename...ArgTypes>
		friend UniquePtr<T> MakeUniqueWithAlloc(IMemoryAllocator* allocator, ArgTypes&&...args);

		//----------------------------------------
		// 作成
		//----------------------------------------

		/// <summary>
		/// 作成したインスタンスとインスタンスを作成したメモリアロケータを指定して作成するコンストラクタ
		/// MakeUniqueからのみ使用することを想定している
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
		/// 空のUniquePtrを作成するコンストラクタ
		/// </summary>
		UniquePtr()
			:ptr_(nullptr)
			, alloc_(nullptr)
		{

		}

		//----------------------------------------
		// 代入
		//----------------------------------------

		/// <summary>
		/// コピーは禁止
		/// </summary>
		/// <param name=""></param>
		UniquePtr(const UniquePtr&) = delete;
		/// <summary>
		/// コピーは禁止
		/// </summary>
		/// <param name=""></param>
		/// <returns></returns>
		UniquePtr& operator=(const UniquePtr&) = delete;

		/// <summary>
		/// ムーブを行う
		/// </summary>
		/// <param name="src"></param>
		UniquePtr(UniquePtr&& src)noexcept
			:alloc_(src.alloc_), ptr_(src.ptr_)
		{
			src.alloc_.Reset();
			src.ptr_ = nullptr;
		}

		/// <summary>
		/// ムーブを行う
		/// </summary>
		/// <param name="src"></param>
		/// <returns></returns>
		UniquePtr& operator=(UniquePtr&& src)noexcept
		{
			// すでに確保してるリソースがあったら開放
			Reset();

			alloc_ = src.alloc_;
			ptr_ = src.ptr_;

			src.alloc_.Reset();
			src.ptr_ = nullptr;

			return *this;
		}


		//----------------------------------------
		// 開放
		//----------------------------------------

		/// <summary>
		/// インスタンスを開放する
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
		// 取得
		//----------------------------------------

		/// <summary>
		/// 管理してるインスタンスのポインタを取得
		/// </summary>
		/// <returns></returns>
		inline Type* Get()const
		{
			return ptr_;
		}

		Type* operator->()const
		{
			assert(ptr_ != nullptr && "nullptrに->を使用しています！");
			return ptr_;
		}

		/// <summary>
		/// 管理してるインスタンスの実際を参照
		/// </summary>
		/// <returns></returns>
		Type& operator*()const
		{
			assert(ptr_ != nullptr && "nullptrの実体にアクセスしようとしています！");
			return *ptr_;
		}

		bool IsActive()const
		{
			return ptr_ != nullptr;
		}

		//----------------------------------------
		// 比較
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
	/// UniquePtrを作成する関数
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

		// メモリを確保してインスタンスを配置する
		void* mem = nullptr;
		mem = allocHolder.Allocate(typeSize);
		new (mem) Type(args...);

		// uniquePtrを作成する
		return UniquePtr<Type>(static_cast<Type*>(mem), allocator);
	}

	template<typename Type, typename...ArgTypes>
	inline UniquePtr<Type> MakeUnique(ArgTypes&&...args)
	{
		return MakeUniqueWithAlloc<Type>(nullptr, args...);
	}
}
