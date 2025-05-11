#pragma once
#include "Memory.hpp"

namespace utl
{
	template<typename Type>
	class Vector
	{
	public:
		//------------------------------------------
		// 作成
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
		// 代入
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

			// 新しいメモリを確保
			begin_ = alloc_.Allocate<Type>(capacity_);
			assert(begin_ != nullptr, "メモリを確保できませんでした");


			for (size_t i = 0; i < size_; i++)
			{
				new (begin_ + i) Type(*(src.begin_ + i));
			}
			return *this;
		}


		/// <summary>
		/// Srcを代入して作成する
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
		/// Srcを代入する
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
		// 開放
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

		void PopBack()
		{
			if (size_ == 0)
			{
				return;
			}
			Resize(size_ - 1);
		}

		//------------------------------------------
		// 追加
		//------------------------------------------

		Type* PushBack(const Type& src)
		{
			// 容量が足りなければ確保する
			if (capacity_ <= size_ + 1)
			{
				ReserveSlack(size_ + 1);
			}

			const size_t insertIndex = size_++;

			Type* ptr = begin_ + insertIndex;

			new (ptr) Type(src);

			++size_;

			return ptr;
		}

		template<typename...ArgTypes>
		Type* EmplaceBack(ArgTypes&&...args)
		{
			// 容量が足りなければ確保する
			if (capacity_ <= size_)
			{
				Reserve(capacity_ * 2);
			}

			const size_t insertIndex = size_;

			Type* ptr = begin_ + insertIndex;

			new (ptr) Type(args...);

			++size_;
			return ptr;
		}

		//------------------------------------------
		// 取得
		//------------------------------------------

		Type* Get(const size_t index)const
		{
			assert(index <= size_ && "範囲外アクセスです");

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

		/// <summary>
		/// 引数の添え字が範囲内か返す関数
		/// </summary>
		/// <param name="index"></param>
		/// <returns></returns>
		bool IsContain(const size_t index)const
		{
			if (index >= size_)
			{
				return false;
			}
			return true;
		}

		//------------------------------------------
		// メモリ操作
		//------------------------------------------

		size_t GetSlack(const size_t begin, const size_t goal)const
		{
			size_t newCapacity = begin;
			if (begin == 0)
			{
				newCapacity = 1;
			}
			// 必要な容量より大きくなるまで繰り返す
			while (newCapacity <= goal)
			{
				// 今の値と今の値を半減した値を足した数をセット
				newCapacity = newCapacity + (newCapacity >> 1);
			}
			return newCapacity;
		}

		template<typename...ArgTypes>
		void Resize(const size_t size, ArgTypes&&...args)
		{
			// もしメモリが足りなければ確保
			if (size > capacity_)
			{
				ReserveSlack(size);
			}

			// 要素数が増えたならその分を初期化
			for (size_t i = size_; i < size; ++i)
			{
				Type* ptr = begin_ + i;
				new (ptr)Type(args...);
			}

			// 要素数が減ったならその分開放
			for (size_t i = size; i < size_; ++i)
			{
				Type* ptr = begin_ + i;
				ptr->~Type();
			}

			size_ = size;
		}

		/// <summary>
		/// キャパシティを要素数にいい感じに合わせる
		/// </summary>
		void FitSlack()
		{
			size_t newCapacity = GetSlack(1, size_);
			ChangeCapacity(newCapacity);
		}

		/// <summary>
		/// キャパシティを要素数に合わせる
		/// </summary>
		void Fit()
		{
			ChangeCapacity(size_);
		}

		/// <summary>
		/// いい感じに余裕をもってキャパシティを拡大する
		/// </summary>
		/// <param name="capacity"></param>
		void ReserveSlack(const size_t capacity)
		{
			// 指定した容量が今と同じかそれ以下だったら何もしない
			if (capacity_ >= capacity)
			{
				return;
			}

			size_t newCapacity = GetSlack(capacity_, capacity);
			ChangeCapacity(newCapacity);
		}

		/// <summary>
		/// キャパシティを拡大
		/// 縮小はしない
		/// </summary>
		/// <param name="capacity"></param>
		void Reserve(size_t capacity)
		{
			if (capacity == 0)
			{
				capacity = 1;
			}
			// 指定した容量が今と同じかそれ以下だったら何もしない
			if (capacity_ >= capacity)
			{
				return;
			}

			ChangeCapacity(capacity);
		}

		/// <summary>
		/// キャパシティを縮小
		/// 拡大はしない
		/// サイズより小さくはならない
		/// </summary>
		/// <param name="capacity"></param>
		void Reduce(const size_t capacity)
		{
			// 指定した容量が今と同じかそれ以下だったら何もしない
			if (capacity_ <= capacity)
			{
				return;
			}
			ChangeCapacity(capacity);
		}

		/// <summary>
		/// キャパシティを変更
		/// サイズより小さくはならない
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
		/// メモリのサイズを変更し、再配置を行う
		/// 容量が要素数より少ない場合、要素は開放される
		/// </summary>
		/// <param name="capacity"></param>
		void ChangeMemorySize(const size_t capacity)
		{
			if (capacity == capacity_)
			{
				return;
			}

			// 新しいメモリを確保
			Type* newBegin = alloc_.Allocate<Type>(capacity);
			assert(newBegin != nullptr, "メモリを確保できませんでした");
			if (newBegin == nullptr)
			{
				return;
			}

			size_t afterSize = size_;
			if (size_ > capacity)
			{
				afterSize = capacity;
			}

			// 新しいメモリに要素をコピー
			for (size_t i = 0; i < afterSize; ++i)
			{
				new (newBegin + i) Type(Move(*(begin_ + i)));
			}

			// 前のメモリのインスタンスを開放
			for (size_t i = 0; i < afterSize; ++i)
			{
				Type* ptr = begin_ + i;
				ptr->~Type();
			}

			// 前のメモリを開放し、新しいメモリをセット
			if (begin_ != nullptr)
			{
				alloc_.Deallocate(begin_);
				begin_ = nullptr;
			}
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
