#pragma once
#include "Vector.h"

namespace utl
{
	template<typename Type>
	class ChunkVector
	{
	public:
		//------------------------------------------
		// 作成
		//------------------------------------------

		ChunkVector(const size_t blockSize, IMemoryAllocator* alloc, IMemoryAllocator* valAlloc)
			: blocks_(alloc)
			, valueMemAlloc_(valAlloc)
			, blockSize_(blockSize)
		{

		}
		ChunkVector(const size_t blockSize, IMemoryAllocator* alloc = nullptr)
			: blocks_(alloc)
			, valueMemAlloc_(alloc)
			, blockSize_(blockSize)
		{

		}

		ChunkVector(const size_t blockSize, const size_t capacity, IMemoryAllocator* alloc, IMemoryAllocator* valAlloc)
			: blocks_(capacity, alloc)
			, valueMemAlloc_(valAlloc)
			, blockSize_(blockSize)
		{

		}
		ChunkVector(const size_t blockSize, const size_t capacity, IMemoryAllocator* alloc = nullptr)
			: blocks_(capacity, alloc)
			, valueMemAlloc_(alloc)
			, blockSize_(blockSize)
		{

		}

		//------------------------------------------
		// 代入
		//------------------------------------------

		/// <summary>
		/// 
		/// </summary>
		/// <param name="src"></param>
		ChunkVector(const ChunkVector& src)
			:blocks_(src.blocks_)
			, valueMemAlloc_(src.valueMemAlloc_.Get())
			, blockSize_(src.blockSize_)
		{

		}
		/// <summary>
		/// 
		/// </summary>
		/// <param name="src"></param>
		/// <returns></returns>
		ChunkVector& operator=(const ChunkVector& src)
		{
			Clear();
			blocks_ = src.blocks_;
			blockSize_ = src.blockSize_;
			valueMemAlloc_ = src.valueMemAlloc_;
			return *this;
		}


		/// <summary>
		/// Srcを代入して作成する
		/// </summary>
		/// <param name="src"></param>
		ChunkVector(ChunkVector&& src)
			:blocks_(src.blocks_)
			, valueMemAlloc_(src.valueMemAlloc_.Get())
			, blockSize_(src.blockSize_)
		{

		}
		/// <summary>
		/// Srcを代入する
		/// </summary>
		/// <param name="src"></param>
		/// <returns></returns>
		ChunkVector& operator=(ChunkVector&& src)
		{
			Clear();
			blocks_ = src.blocks_;
			blockSize_ = src.blockSize_;
			valueMemAlloc_ = src.valueMemAlloc_;
			return *this;
		}

		//------------------------------------------
		// 開放
		//------------------------------------------

		~ChunkVector()
		{
			Clear();
		}

		void Clear()
		{
			blocks_.Clear();
		}

		void PopBack()
		{
			if (blocks_.Empty())
			{
				return;
			}
			// 末尾のブロックの末尾の要素をポップ
			blocks_.Back()->PopBack();
			// 末尾のブロックが空になったら末尾のブロックをポップ
			if (blocks_.Back()->Empty())
			{
				blocks_.PopBack();
			}
		}

		//------------------------------------------
		// 追加
		//------------------------------------------

		Type* PushBack(const Type& src)
		{
			// ブロックが無ければ作成
			if (blocks_.Empty())
			{
				blocks_.EmplaceBack(blockSize_, valueMemAlloc_.Get());
			}
			// 末尾のブロックに空きがなければ作成
			if (blocks_.Back()->Size() >= blockSize_)
			{
				blocks_.EmplaceBack(blockSize_, valueMemAlloc_.Get());
			}
			return blocks_.Back()->PushBack(src);
		}

		template<typename...ArgTypes>
		Type* EmplaceBack(ArgTypes&&...args)
		{
			// ブロックが無ければ作成
			if (blocks_.Empty())
			{
				blocks_.EmplaceBack(blockSize_, valueMemAlloc_.Get());
			}
			// 末尾のブロックに空きがなければ作成
			if (blocks_.Back()->Size() >= blockSize_)
			{
				blocks_.EmplaceBack(blockSize_, valueMemAlloc_.Get());
			}
			return blocks_.Back()->EmplaceBack(args...);
		}

		//------------------------------------------
		// 取得
		//------------------------------------------

		inline constexpr size_t GetBlockIndex(const size_t index)const
		{
			return index / blockSize_;
		}

		inline constexpr size_t GetValueIndex(const size_t index)const
		{
			return index % blockSize_;
		}

		inline constexpr size_t GetBlockSize()const
		{
			return blockSize_;
		}

		const Vector<Type>* GetBLock(const size_t blockIndex)const
		{
			return blocks_.Get(blockIndex);
		}

		Type* Get(const size_t index)const
		{
			const size_t blockIndex = GetBlockIndex(index);
			const size_t valueIndex = GetValueIndex(index);

			const Vector<Type>* backBlock = GetBLock(blockIndex);
			assert(backBlock != nullptr && "範囲外アクセスです");

			Type* ptr = backBlock->Get(valueIndex);
			assert(ptr != nullptr && "範囲外アクセスです");

			return ptr;
		}

		Type& operator[](const size_t index)const
		{
			return blocks_[GetBlockIndex(index)][GetValueIndex(index)];
		}
		Type& operator[](const size_t index)
		{
			return blocks_[GetBlockIndex(index)][GetValueIndex(index)];
		}

		Type* Begin()const
		{
			if (blocks_.Empty())
			{
				return nullptr;
			}
			if (blocks_.Begin()->Empty())
			{
				return nullptr;
			}
			return blocks_.Begin()->Begin();
		}

		Type* Back()const
		{
			if (blocks_.Empty())
			{
				return nullptr;
			}
			if (blocks_.Back()->Empty())
			{
				return nullptr;
			}
			return blocks_.Back()->Back();
		}

		size_t Size()const
		{
			size_t result = 0;
			if (blocks_.Empty())
			{
				return result;
			}
			if (blocks_.Size() >= 2)
			{
				result += (blocks_.Size() - 1) * blockSize_;
			}
			result += blocks_.Back()->Size();

			return result;
		}

		size_t Capacity()const
		{
			return blocks_.Capacity() * blockSize_;
		}

		bool Empty()const
		{
			if (blocks_.Empty())
			{
				return true;
			}
			return blocks_.Back()->Empty();
		}

		/// <summary>
		/// 引数の添え字が範囲内か返す関数
		/// </summary>
		/// <param name="index"></param>
		/// <returns></returns>
		bool IsContain(const size_t index)const
		{
			const size_t blockIndex = GetBlockIndex(index);
			const size_t valueIndex = GetValueIndex(index);

			if (!blocks_.IsContain(blockIndex))
			{
				return false;
			}

			return blocks_[blockIndex].IsContain(valueIndex);
		}

		//------------------------------------------
		// メモリ操作
		//------------------------------------------

		//size_t GetSlack(const size_t begin, const size_t goal)const
		//{
		//}

		template<typename...ArgTypes>
		void Resize(const size_t size, ArgTypes&&...args)
		{
			if (size == 0)
			{
				Clear();
				return;
			}

			size_t beforeBlockNum = blocks_.Size();
			size_t afterBlockNum = GetBlockIndex(size - 1) + 1;
			// ブロックの数を変更
			blocks_.Resize(afterBlockNum, blockSize_, valueMemAlloc_.Get());
			// ブロックが増えたなら初期化する
			for (size_t i = beforeBlockNum; i < afterBlockNum; i++)
			{
				blocks_[i].Resize(blockSize_, args...);
			}

			// 末尾のブロックの整合性を取る
			blocks_.Back()->Resize(GetValueIndex(size - 1) + 1, args...);
		}



	private:

		Vector<Vector<Type>> blocks_;
		MemoryAllocatorHolder valueMemAlloc_;
		size_t blockSize_;

	};
}