#pragma once
#include "../Vector/Vector.h"
#include <utility>

namespace utl
{
	using DenceKey = uint32_t;
	using DenceIndex = uint32_t;
	// NullのDenceKey,DenceIndexを表す定数
	constexpr uint32_t NULL_DENCE = UINT32_MAX;

	/// <summary>
	/// 連番の整数をキーとし、削除するときに最後の要素を削除する要素に上書きするテーブル
	/// </summary>
	class SparseSet
	{
	private:
		//struct KeyIndex
		//{
		//	DenceKey key = NULL_DENCE;
		//	DenceIndex index = NULL_DENCE;
		//};
	public:

		//-----------------------------------
		// 作成
		//-----------------------------------

		SparseSet(IMemoryAllocator* alloc = nullptr)
			//:denceSparce_(alloc)
			:indexToKey_(alloc)
			, keyToIndex_(alloc)
			, size_(0)
		{
			// clang
		}

		SparseSet(const SparseSet& src)
			:indexToKey_(src.indexToKey_)
			, keyToIndex_(src.keyToIndex_)
			, size_(src.size_)
		{

		}
		SparseSet& operator=(const SparseSet& src)
		{
			Reset();
			indexToKey_ = src.indexToKey_;
			keyToIndex_ = src.keyToIndex_;
			size_ = src.size_;
		}

		SparseSet(SparseSet&& src)noexcept
			:indexToKey_(src.indexToKey_)
			, keyToIndex_(src.keyToIndex_)
			, size_(src.size_)
		{

		}
		SparseSet& operator=(SparseSet&& src)noexcept
		{
			Reset();
			indexToKey_ = src.indexToKey_;
			keyToIndex_ = src.keyToIndex_;
			size_ = src.size_;
		}

		//-----------------------------------
		// 開放
		//-----------------------------------

		~SparseSet()
		{
			Reset();
		}

		void Reset()
		{
			//denceSparce_.Clear();
			keyToIndex_.Clear();
			indexToKey_.Clear();
			size_ = 0;
		}


		//-----------------------------------
		// 追加
		//-----------------------------------

		DenceIndex Insert(const DenceKey key)
		{
			// すでに追加済みのキーならそのまま返す
			if (CheckActive(key))
			{
				return keyToIndex_[key];
				//KeyIndex& info = denceSparce_[key];
				//return info.index;
			}

			// 添え字を求めた後に要素数を増加
			DenceIndex index = size_++;

			// まだ追加してなければ追加
			if (!keyToIndex_.IsContain(key))
			{
				keyToIndex_.Resize(key + 1, NULL_DENCE);
			}
			if (!indexToKey_.IsContain(index))
			{
				indexToKey_.Resize(index + 1, NULL_DENCE);
			}
			keyToIndex_[key] = index;
			indexToKey_[index] = key;

			return index;
		}

		DenceIndex operator[](const DenceKey key)
		{
			return Insert(key);
		}

		//-----------------------------------
		// 取得
		//-----------------------------------

		bool CheckActive(const DenceKey key)const
		{
			if (key == NULL_DENCE)
			{
				return false;
			}
			if (!keyToIndex_.IsContain(key))
				//if (!denceSparce_.IsContain(key))
			{
				return false;
			}
			if (!indexToKey_.IsContain(keyToIndex_[key]))
				//if (!denceSparce_.IsContain(key))
			{
				return false;
			}
			//return denceSparce_[key].index != NULL_DENCE;
			return keyToIndex_[key] != NULL_DENCE;
		}

		DenceIndex GetIndex(const DenceKey key)const
		{
			if (!keyToIndex_.IsContain(key))
			{
				return NULL_DENCE;
			}
			//if (!CheckActive(key))
			//{
			//	return NULL_DENCE;
			//}
			return keyToIndex_[key];
			//return denceSparce_[key].index;
		}

		DenceKey GetKey(const DenceIndex index)const
		{
			//if (!denceSparce_.IsContain(index))
			if (!keyToIndex_.IsContain(index))
			{
				return NULL_DENCE;
			}

			return keyToIndex_[index];
			//return denceSparce_[index].key;
		}

		size_t Size()const
		{
			return size_;
		}

		//-----------------------------------
		// メモリ操作
		//-----------------------------------

		void FitSlack()
		{
			//denceSparce_.FitSlack();
			indexToKey_.FitSlack();
			keyToIndex_.FitSlack();
		}

		void Fit()
		{
			//denceSparce_.Fit();
			indexToKey_.Fit();
			keyToIndex_.Fit();
		}

		//-----------------------------------
		// 削除
		//-----------------------------------

		DenceIndex Remove(const DenceKey removeKey)
		{
			if (!CheckActive(removeKey))
			{
				return NULL_DENCE;
			}

			const DenceIndex removeIndex = keyToIndex_[removeKey];
			//const DenceIndex removeIndex = denceSparce_[removeKey].index;

			const DenceIndex backIndex = size_ - 1;
			const DenceKey backKey = indexToKey_[backIndex];
			//const DenceKey backKey = denceSparce_[backIndex].key;

			// 削除する要素の添え字と最後の要素の添え字が異なれば、
			if (removeIndex != backIndex)
			{
				// 削除する要素に最後の要素を張り付ける
				indexToKey_[removeIndex] = backKey;
				keyToIndex_[backKey] = removeIndex;
				//denceSparce_[removeIndex].key = backKey;
				//denceSparce_[backKey].index = removeIndex;
			}

			// 削除する要素のキーと添え字を無効化
			keyToIndex_[removeKey] = NULL_DENCE;
			indexToKey_[backIndex] = NULL_DENCE;
			//denceSparce_[removeKey].index = NULL_DENCE;
			//denceSparce_[backIndex].key = NULL_DENCE;
			--size_;

			// 位置が移動した可能性がある最後の要素の添え字を返す
			return backIndex;
		}

	private:

		// KeyToIndexとIndexToKeyを兼ねるコンテナ
		//Vector<KeyIndex> denceSparce_;
		Vector<DenceIndex> keyToIndex_;
		Vector<DenceKey> indexToKey_;
		// 登録したキーの数
		size_t size_;

	};

	template<typename Type>
	class DenceMap
	{
	public:
		//------------------------------------------
		// 作成
		//------------------------------------------

		DenceMap(IMemoryAllocator* alloc = nullptr)
			:table_(alloc)
			, values_(alloc)
		{

		}
		DenceMap(const size_t capacity, IMemoryAllocator* alloc = nullptr)
			:table_(alloc)
			, values_(capacity, alloc)
		{

		}

		//------------------------------------------
		// 追加
		//------------------------------------------


		Type* Insert(const DenceKey key, const Type& src)
		{
			DenceIndex index = NULL_DENCE;

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
		Type* Insert(const DenceKey key, ArgTypes&&...args)
		{
			DenceIndex index = NULL_DENCE;

			// すでにキーが追加されていたら上書きする
			if (table_.CheckActive(key))
			{
				index = table_[key];
				//values_[index] = Type(args...);
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

		void Remove(const DenceKey removeKey)
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

		Type* Get(const DenceKey key)const
		{
			if (!table_.CheckActive(key))
			{
				return nullptr;
			}
			DenceIndex index = table_.GetIndex(key);
			return &values_[index];
		}

		size_t Size()const
		{
			return table_.Size();
		}

		Type& operator[](const DenceKey key)
		{
			DenceIndex index = NULL_DENCE;
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

		Vector<Type> values_;

	};
}

