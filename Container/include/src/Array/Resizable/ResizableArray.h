#pragma once
#include "Memory.hpp"

namespace utl
{
	/// <summary>
	/// 要素数やメモリ確保のタイミングなどを動的に指定して作成する固定長の配列
	/// </summary>
	/// <typeparam name="Type"></typeparam>
	template<typename Type>
	class ResizableArray
	{
	public:

		//--------------------------------------
		// 作成
		//--------------------------------------

		/// <summary>
		/// 空のDynamicArrayを作成する
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
			Release();

			alloc_.Reset(alloc);
			begin_ = static_cast<Type*>(alloc_.Allocate(size * sizeof(Type)));
			size_ = size;

			for (size_t i = 0; i < size; ++i)
			{
				Type* ptr = begin_ + i;
				new (ptr) Type(args...);
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
		// 開放
		//--------------------------------------

		void Release()
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
		}


		~ResizableArray()
		{
			Release();
		}

		//--------------------------------------
		// 取得
		//--------------------------------------

		Type& operator[](const size_t index)const
		{
			return *Get(index);
		}

		Type* Data()const
		{
			return begin_;
		}

		const size_t Size()const
		{
			return size_;
		}

		Type* Get(const size_t index)const
		{
			assert(index < size_ && "範囲外アクセスです");

			return begin_ + index;
		}

	private:

		MemoryAllocatorHolder alloc_;

		Type* begin_;

		size_t size_;

	};
}

