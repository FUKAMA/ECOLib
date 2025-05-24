#pragma once
#include "../Vector/Vector.h"
#include <utility>

namespace utl
{
	/// <summary>
	/// まばらなキー
	/// </summary>
	using SparseKey = uint32_t;

	/// <summary>
	/// 密集した添え字
	/// </summary>
	using DenceIndex = uint32_t;

	// NullのDenceKey,DenceIndexを表す定数
	constexpr uint32_t NULL_VALUE = UINT32_MAX;

	/// <summary>
	/// 連番の整数をキーとし、削除するときに最後の要素を削除する要素に上書きするテーブル
	/// </summary>
	class SparseSet
	{
	public:

		//-----------------------------------
		// 作成
		//-----------------------------------

		SparseSet(IMemoryAllocator* alloc = nullptr)
			:indexToKey_(alloc)
			, keyToIndex_(alloc)
			, size_(0)
		{

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
			keyToIndex_.Clear();
			indexToKey_.Clear();
			size_ = 0;
		}


		//-----------------------------------
		// 追加
		//-----------------------------------

		DenceIndex Insert(const SparseKey key)
		{
			// すでに追加済みのキーならそのまま返す
			if (CheckActive(key))
			{
				return keyToIndex_[key];
			}

			// 添え字を求めた後に要素数を増加
			DenceIndex index = size_++;

			// まだ追加してなければ追加
			if (!keyToIndex_.IsContain(key))
			{
				keyToIndex_.Resize(key + 1, NULL_VALUE);
			}
			if (!indexToKey_.IsContain(index))
			{
				indexToKey_.Resize(index + 1, NULL_VALUE);
			}
			keyToIndex_[key] = index;
			indexToKey_[index] = key;

			return index;
		}

		DenceIndex operator[](const SparseKey key)
		{
			return Insert(key);
		}

		//-----------------------------------
		// 取得
		//-----------------------------------

		bool CheckActive(const SparseKey key)const
		{
			if (key == NULL_VALUE)
			{
				return false;
			}
			if (!keyToIndex_.IsContain(key))
			{
				return false;
			}
			if (!indexToKey_.IsContain(keyToIndex_[key]))
			{
				return false;
			}
			return keyToIndex_[key] != NULL_VALUE;
		}

		DenceIndex GetIndex(const SparseKey key)const
		{
			if (!keyToIndex_.IsContain(key))
			{
				return NULL_VALUE;
			}
			return keyToIndex_[key];
		}

		SparseKey GetKey(const DenceIndex index)const
		{
			if (!keyToIndex_.IsContain(index))
			{
				return NULL_VALUE;
			}

			return keyToIndex_[index];
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
			indexToKey_.FitSlack();
			keyToIndex_.FitSlack();
		}

		void Fit()
		{
			indexToKey_.Fit();
			keyToIndex_.Fit();
		}

		//-----------------------------------
		// 削除
		//-----------------------------------

		DenceIndex Remove(const SparseKey removeKey)
		{
			if (!CheckActive(removeKey))
			{
				return NULL_VALUE;
			}

			const DenceIndex removeIndex = keyToIndex_[removeKey];

			const DenceIndex backIndex = size_ - 1;
			const SparseKey backKey = indexToKey_[backIndex];

			// 削除する要素の添え字と最後の要素の添え字が異なれば、
			if (removeIndex != backIndex)
			{
				// 削除する要素に最後の要素を張り付ける
				indexToKey_[removeIndex] = backKey;
				keyToIndex_[backKey] = removeIndex;
			}

			// 削除する要素のキーと添え字を無効化
			keyToIndex_[removeKey] = NULL_VALUE;
			indexToKey_[backIndex] = NULL_VALUE;
			--size_;

			// 位置が移動した可能性がある最後の要素の添え字を返す
			return backIndex;
		}

	private:

		Vector<DenceIndex> keyToIndex_;
		Vector<SparseKey> indexToKey_;
		// 登録したキーの数
		size_t size_;

	};


}