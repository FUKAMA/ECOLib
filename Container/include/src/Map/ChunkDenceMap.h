#pragma once
#include "../Set/SparseSet.h"
#include "../Vector/ChunkVector.h"

namespace utl
{
	template<typename Type>
	class ChunkDenceMap
	{
	public:
		ChunkDenceMap(const size_t blockSize, IMemoryAllocator* alloc, IMemoryAllocator* valAlloc)
			:table_(alloc)
			, values_(blockSize, alloc, valAlloc)
		{

		}
		ChunkDenceMap(const size_t blockSize, IMemoryAllocator* alloc = nullptr)
			:table_(alloc)
			, values_(blockSize, alloc, alloc)
		{

		}



		//------------------------------------------
		// 追加
		//------------------------------------------


		Type* Insert(const SparseKey key, const Type& src)
		{
			DenceIndex index = NULL_VALUE;

			// すでにキーが追加されていたら上書きする
			if (table_.CheckActive(key))
			{
				index = table_[key];
				values_[index] = src;
				return &values_[index];
			}

			index = table_.Insert(key);
			// 要素をコンテナに追加
			// 有効な要素は前詰めで配置されるため、末尾に追加すればいい
			return values_.EmplaceBack(src);
		}

		template<typename...ArgTypes>
		Type* Insert(const SparseKey key, ArgTypes&&...args)
		{
			DenceIndex index = NULL_VALUE;

			// すでにキーが追加されていたら上書きする
			if (table_.CheckActive(key))
			{
				index = table_[key];
				values_[index] = Type(args...);
				return &values_[index];
			}

			index = table_.Insert(key);
			// 要素をコンテナに追加
			// 有効な要素は前詰めで配置されるため、末尾に追加すればいい
			return values_.EmplaceBack(args...);
		}

		//------------------------------------------
		// 削除
		//------------------------------------------

		void Remove(const SparseKey removeKey)
		{
			// すでに無効なら何もしない
			if (!table_.CheckActive(removeKey))
			{
				return;
			}

			DenceIndex remIndex = table_.GetIndex(removeKey);

			// テーブルからキーを削除し、場所が入れ替わったキーと添え字を取得
			DenceIndex backIndex = table_.Remove(removeKey);

			// 削除する要素の末尾の要素を上書き
			if (remIndex != backIndex)
			{
				values_[remIndex] = std::move(values_[backIndex]);
			}

			// 末尾の要素を無効化
			values_.PopBack();
		}

		void Clear()
		{
			table_.Reset();
			values_.Clear();
		}

		//------------------------------------------
		// メモリ操作
		//------------------------------------------

		void Fit()
		{
			table_.Fit();
			values_.Fit();
		}

		void FitSlack()
		{
			table_.FitSlack();
			values_.FitSlack();
		}

		//------------------------------------------
		// 取得
		//------------------------------------------

		Type* Data()const
		{
			return values_.Begin();
		}

		Type* Get(const SparseKey key)const
		{
			if (!table_.CheckActive(key))
			{
				return nullptr;
			}
			DenceIndex index = table_.GetIndex(key);
			return &values_[index];
		}

		inline constexpr size_t GetBlockSize()const
		{
			return values_.GetBlockSize();
		}
		
		size_t Size()const
		{
			return table_.Size();
		}

		Type& operator[](const SparseKey key)
		{
			DenceIndex index = NULL_VALUE;
			if (table_.CheckActive(key))
			{
				index = table_[key];
				return values_[index];
			}

			index = table_.Insert(key);
			// 要素をコンテナに追加
			// 有効な要素は前詰めで配置されるため、末尾に追加すればいい
			return *values_.EmplaceBack();
		}

	private:

		SparseSet table_;
		ChunkVector<Type> values_;

	};
}