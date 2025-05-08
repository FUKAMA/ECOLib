#pragma once

namespace utl
{
	/// <summary>
	/// メモリを確保する手段を外部に提供するインターフェースクラス
	/// </summary>
	class IMemoryAllocator
	{
	protected:
		friend class MemoryAllocatorHolder;

		/// <summary>
		/// 空のメモリを確保する処理を実装する仮想関数
		/// 確保したメモリはvoid*だけで開放できる必要がある
		/// </summary>
		/// <param name="size"></param>
		/// <returns></returns>
		virtual void* Allocate(const size_t size) = 0;

		/// <summary>
		/// 確保したメモリを開放する関数を実装する仮想関数
		/// </summary>
		/// <param name="allocated"></param>
		virtual void Deallocate(void* allocated) = 0;

	};

	/// <summary>
	/// メモリアロケータの参照を受け取るときに便利なクラス
	/// メモリアロケータが指定されてればそれを使い、
	/// 指定されてなければC++標準の手段を使う
	/// </summary>
	class MemoryAllocatorHolder
	{
	public:
		MemoryAllocatorHolder(IMemoryAllocator* alloc = nullptr)
			:alloc_(alloc)
		{
		}

		MemoryAllocatorHolder(const MemoryAllocatorHolder& src)
			:alloc_(src.alloc_)
		{
		}

		IMemoryAllocator* Get()const
		{
			return alloc_;
		}

		[[nodiscard]] void* Allocate(const size_t size);

		/// <summary>
		/// 特定の型のインスタンスを任意の個数格納できるサイズの空メモリを確保する
		/// </summary>
		/// <typeparam name="Type"></typeparam>
		/// <param name="num"></param>
		/// <returns></returns>
		template<typename Type>
		[[nodiscard]] Type* Allocate(const size_t num = 1)
		{
			void* mem = Allocate(sizeof(Type) * num);
			return static_cast<Type*>(mem);
		}
		void Deallocate(void* allocated);

		void Reset(IMemoryAllocator* alloc = nullptr)
		{
			alloc_ = alloc;
		}

	private:

		IMemoryAllocator* alloc_;

	};

	/// <summary>
	/// メモリアロケータの実装を特に指定しなかった時に使用するためのクラス
	/// C++標準の手段でメモリを確保、開放する
	/// </summary>
	class DefaultMemoryAllocator :public IMemoryAllocator
	{
		/// <summary>
		/// mallocで空のメモリを確保する
		/// </summary>
		/// <param name="size"></param>
		/// <returns></returns>
		virtual void* Allocate(const size_t size)override final;

		/// <summary>
		/// mallocで確保したメモリをfreeで開放する
		/// </summary>
		/// <param name="allocated"></param>
		virtual void Deallocate(void* allocated)override final;

	};

}
