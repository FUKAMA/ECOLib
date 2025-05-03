#pragma once
#include "../Allocator/MemoryAllocator.h"

namespace utl
{
	///// <summary>
	///// 参照カウンタとオブジェクトのポインタを管理するクラス
	///// 管理するオブジェクトとこのインスタンスを格納するメモリは引数で渡すMemoryAllocatorで作成する
	///// </summary>
	///// <typeparam name="Type"></typeparam>
	//template<typename Type>
	//class RefCounter
	//{
	//private:
	//	template<typename T, typename...ArgTypes>
	//	friend RefCounter<T>* MakeRefCounterWithAlloc(IMemoryAllocator* alloc, ArgTypes&&...args);

	//	RefCounter(Type* ptr, IMemoryAllocator* alloc)
	//		:ptr_(ptr)
	//		, alloc_(alloc)
	//		, counter_(0)
	//	{

	//	}

	//public:

	//	RefCounter()
	//		:ptr_(nullptr)
	//		, alloc_(nullptr)
	//		, counter_(0)
	//	{

	//	}

	//	~RefCounter() = default;

	//	void Look()
	//	{
	//		++counter_;
	//	}

	//	void UnLook()
	//	{

	//		--counter_;
	//	}

	//	int Counter()const
	//	{
	//		return counter_;
	//	}

	//	Type* GetPtr()const
	//	{
	//		return ptr_;
	//	}

	//	MemoryAllocatorHolder& GetAlloc()const
	//	{
	//		return alloc_;
	//	}

	//	void Reset()
	//	{
	//		if (counter_ > 0 || ptr_ == nullptr)
	//		{
	//			return;
	//		}

	//		ptr_->~Type();
	//		alloc_.Deallocate(ptr_);
	//		ptr_ = nullptr;
	//		counter_ = 0;
	//	}

	//private:

	//	Type* ptr_;

	//	MemoryAllocatorHolder alloc_;

	//	int counter_;

	//};

	//template<typename Type, typename...ArgTypes>
	//RefView<Type> MakeRefCounterWithAlloc(IMemoryAllocator* alloc, ArgTypes&&...args)
	//{
	//	MemoryAllocatorHolder allocHolder(alloc);

	//	void* mem = allocHolder.Allocate(sizeof(Type));
	//	new(mem) Type(args...);
	//	Type* ptr = static_cast<Type*>(mem);

	//	// Counterを作る
	//	void* counterMem = allocHolder.Allocate(sizeof(RefCounter<Type>));
	//	new (counterMem) RefCounter<Type>(ptr, alloc);

	//	// Viewを作る
	//	return RefView<Type>(counterMem);
	//}

	//template<typename Type>
	//class RefView
	//{
	//public:

	//	RefView(RefCounter<Type>* ref)
	//		:ref_(ref)
	//	{
	//		ref->Look();
	//	}

	//	RefView(const RefView& src)
	//		:ref_(src.ref_)
	//	{
	//		ref_->Look();
	//	}

	//	~RefView()
	//	{
	//		Reset();
	//	}

	//	void Reset()
	//	{
	//		if (ref_ == nullptr)
	//		{
	//			return;
	//		}

	//		ref_->UnLook();
	//		if (ref_->Counter() > 0)
	//		{
	//			return;
	//		}
	//		ref_->Reset();
	//		MemoryAllocatorHolder allocHolder = ref_->GetAlloc();

	//		allocHolder.Deallocate(ref_);

	//		ref_ = nullptr;
	//	}

	//private:

	//	RefCounter<Type>* ref_;

	//};
}
