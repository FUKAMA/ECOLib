#pragma once
#include <cassert>
#include "../../Allocator/MemoryAllocator.h"

namespace utl
{
	template<typename Type>
	class SharedPtr;

	/// <summary>
	/// 参照カウンタとオブジェクトのポインタを管理するクラス
	/// 管理するオブジェクトとこのインスタンスを格納するメモリは引数で渡すMemoryAllocatorで作成する
	/// このクラスのインスタンスを開放するときは、コンストラクタに渡したメモリアロケータで開放する必要がある
	/// </summary>
	/// <typeparam name="Type"></typeparam>
	template<typename Type>
	class RefCounter
	{
	private:
		//---------------------------------
		// 作成
		//---------------------------------

		template<typename T, typename...ArgTypes>
		friend RefCounter<T>* MakeRefCounterWithAlloc(IMemoryAllocator* alloc, ArgTypes&&...args);

		/// <summary>
		/// 有効なRefCounterを作成するコンストラクタ
		/// MakeRefCounterWithAllocから作成されることを想定している。
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
		/// 空のRefCounterを作成するコンストラクタ
		/// </summary>
		RefCounter()
			:ptr_(nullptr)
			, alloc_(nullptr)
			, counter_(0)
		{

		}

		//---------------------------------
		// 開放
		//---------------------------------

		~RefCounter() = default;

		//---------------------------------
		// カウント
		//---------------------------------

		/// <summary>
		/// 参照カウンタを増加
		/// </summary>
		void Look()
		{
			++counter_;
		}

		/// <summary>
		/// 参照カウンタを現象
		/// メモリの開放は外部に任せる
		/// </summary>
		void UnLook()
		{
			--counter_;
		}

		/// <summary>
		/// 現在参照している数を返す
		/// </summary>
		/// <returns></returns>
		int Counter()const
		{
			return counter_;
		}

		/// <summary>
		/// 管理してるインスタンスのポインタを取得
		/// </summary>
		/// <returns></returns>
		Type* GetPtr()const
		{
			return ptr_;
		}

		/// <summary>
		/// このクラスと管理してるインスタンスのメモリを管理するアロケータのポインタを取得
		/// </summary>
		/// <returns></returns>
		MemoryAllocatorHolder& GetAlloc()
		{
			return alloc_;
		}

		/// <summary>
		/// 確保したインスタンスを開放する
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
	/// 参照カウンタとインスタンスのメモリを並べて配置するための構造体
	/// ポインタとしてのみの利用を目的とし、インスタンス化は許さない
	/// Typeのインスタンスを開放したら両方開放されるようにInsを先頭に配置
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
	/// メモリアロケータを使ってRefCounterを作成する関数
	/// RefCounterとRefCounterが管理するインスタンスの両方をメモリアロケータで確保したメモリに配置する
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

		// RefCounterToInsを使い、RefCounterとTypeのメモリを同時に確保
		void* mem = allocHolder.Allocate(sizeof(RefCounterToIns<Type>));
		RefCounterToIns<Type>* allocResult = static_cast<RefCounterToIns<Type>*>(mem);
		// 確保したメモリにインスタンスを作成
		new (&allocResult->ins) Type(args...);
		new (&allocResult->typeCounter) RefCounter<Type>(&allocResult->ins, alloc);

		return &allocResult->typeCounter;
	}

	/// <summary>
	/// 共有ポインタ
	/// </summary>
	/// <typeparam name="Type"></typeparam>
	template<typename Type>
	class SharedPtr
	{
	private:

		//---------------------------------
		// 作成
		//---------------------------------

		template<typename T, typename...ArgTypes>
		friend SharedPtr<T> MakeSharedWithAlloc(IMemoryAllocator* alloc, ArgTypes&&...args);

		/// <summary>
		/// リソースのカウンタを渡して作成するコンストラクタ
		/// </summary>
		/// <param name="ref"></param>
		SharedPtr(RefCounter<Type>* ref)
			:ref_(ref)
		{
			ref->Look();
		}

	public:

		/// <summary>
		/// 空の共有ポインタを作成するコンストラクタ
		/// </summary>
		SharedPtr()
			:ref_(nullptr)
		{

		}


		//---------------------------------
		// 代入
		//---------------------------------

		/// <summary>
		/// 共有ポインタをコピーして作成するコンストラクタ
		/// 共有カウントは増える
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
		/// 共有ポインタをコピーする
		/// 共有カウントは増える
		/// すでに管理してたら参照を外す
		/// </summary>
		/// <param name="src"></param>
		/// <returns></returns>
		SharedPtr<Type>& operator=(const SharedPtr& src)
		{
			// 自分自身を代入してないか確認
			if (this == &src || this->ref_ == src.ref_)
			{
				return *this;
			}

			// すでに扱ってるリソースがあったら開放
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
		/// 共有ポインタの所有権を移動して作成するコンストラクタ
		/// </summary>
		/// <param name="src"></param>
		SharedPtr(SharedPtr<Type>&& src)noexcept
			:ref_(src.ref_)
		{
			src.ref_ = nullptr;
		}
		/// <summary>
		/// 共有ポインタの所有権を移動する
		/// すでに管理してたら参照を外す
		/// </summary>
		/// <param name="src"></param>
		/// <returns></returns>
		SharedPtr<Type>& operator=(SharedPtr<Type>&& src)noexcept
		{
			// すでに扱ってるリソースがあったら開放
			Reset();

			ref_ = src.ref_;
			src.ref_ = nullptr;
			return *this;
		}


		//---------------------------------
		// 開放
		//---------------------------------

		/// <summary>
		/// 参照をやめる
		/// インスタンスを参照してる存在がいなくなれば開放する
		/// </summary>
		void Reset()
		{
			if (ref_ == nullptr)
			{
				return;
			}

			ref_->UnLook();

			// カウンタが０以下になったらインスタンスを開放する
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
		// 取得
		//---------------------------------

		/// <summary>
		/// 管理してるインスタンスのポインタを返す
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
		/// 管理してるインスタンスのポインタにアクセスする
		/// </summary>
		/// <returns></returns>
		Type* operator->()const
		{
			assert(ref_ != nullptr && "nullptrの実体にアクセスしようとしています");
			assert(ref_->GetPtr() != nullptr && "nullptrの実体にアクセスしようとしています");
			return Get();
		}

		/// <summary>
		/// 管理しているインスタンスの実体を返す
		/// </summary>
		/// <returns></returns>
		Type& operator*()const
		{
			assert(ref_ != nullptr && "nullptrの実体にアクセスしようとしています");
			return *Get();
		}

		/// <summary>
		/// 有効なインスタンスを管理しているか
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
		// 比較
		//---------------------------------

		/// <summary>
		/// 生ポインタと管理しているインスタンスのポインタが一致すればtrue
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
		/// SharedPtrが同じインスタンスを管理していればtrue
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
		/// 生ポインタと管理しているインスタンスのポインタが一致しなければtrue
		/// </summary>
		/// <param name="ptr"></param>
		/// <returns></returns>
		const bool operator!=(const Type* ptr)const
		{
			return !((*this) == ptr);
		}
		/// <summary>
		/// SharedPtrが同じインスタンスを管理していなければtrue
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
	/// アロケータを指定してSharedPtrを作成する
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
		// Viewを作る
		return SharedPtr<Type>(counter);
	}

	/// <summary>
	/// SharedPtrを作成する
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
