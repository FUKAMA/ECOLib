#pragma once
#include "../Vector/Vector.h"

namespace utl
{
	using SwapKey = uint32_t;
	using SwapIndex = uint32_t;
	constexpr uint32_t NULL_SWAP_INDEX = UINT32_MAX;

	/// <summary>
	/// 連番の整数をキーとし、削除するときに最後の要素を削除する要素に上書きするテーブル
	/// </summary>
	class DenceTable
	{
		struct KeyIndex
		{
			SwapKey key = 0;
			SwapIndex index = NULL_SWAP_INDEX;
		};
	public:

		//-----------------------------------
		// 作成
		//-----------------------------------

		DenceTable(IMemoryAllocator* alloc = nullptr)
			:linkInfo_(alloc)
			, size_(0)
		{

		}

		//-----------------------------------
		// 開放
		//-----------------------------------

		~DenceTable()
		{
			Reset();
		}

		void Reset()
		{
			linkInfo_.Clear();
			size_ = 0;
		}


		//-----------------------------------
		// 追加
		//-----------------------------------

		SwapIndex Insert(const SwapKey key)
		{
			// すでに追加済みのキーならそのまま返す
			if (CheckActive(key))
			{
				KeyIndex& info = linkInfo_[key];
				return info.index;
			}

			// 添え字を求めながら要素数を増加
			SwapIndex index = ++size_;

			// まだ追加してなければ追加
			linkInfo_.Resize(key);
			KeyIndex& keyInfo = linkInfo_[key];
			KeyIndex& indexInfo = linkInfo_[index];
			keyInfo.index = index;
			indexInfo.key = key;

			return index;
		}

		SwapIndex operator[](const SwapKey key)
		{
			return Insert(key);
		}

		//-----------------------------------
		// 取得
		//-----------------------------------

		bool CheckActive(const SwapKey key)const
		{
			if (!linkInfo_.IsContain(key))
			{
				return false;
			}
			return linkInfo_[key].index != NULL_SWAP_INDEX;
		}

		SwapIndex Get(const SwapKey key)const
		{
			if (!CheckActive(key))
			{
				return NULL_SWAP_INDEX;
			}
			return linkInfo_[key].index;
		}

		//-----------------------------------
		// 削除
		//-----------------------------------

		SwapIndex Remove(const SwapKey removeKey)
		{
			if (!CheckActive(removeKey))
			{
				return NULL_SWAP_INDEX;
			}

			const SwapIndex removeIndex = linkInfo_[removeKey].index;

			const SwapIndex backIndex = size_ - 1;
			const SwapKey backKey = linkInfo_[backIndex].key;

			// 削除する要素の添え字と最後の要素の添え字が異なれば、
			if (removeIndex != backIndex)
			{
				// 削除する要素に最後の要素を張り付ける
				linkInfo_[removeIndex].key = backKey;
				linkInfo_[removeKey].index = backIndex;
			}

			// 最後の要素を無効に
			linkInfo_[backKey].index = NULL_SWAP_INDEX;
			--size_;

			// 位置が移動した可能性がある最後の要素の添え字を返す
			return backIndex;
		}

	private:

		// KeyToIndexとIndexToKeyを兼ねるコンテナ
		Vector<KeyIndex> linkInfo_;
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

		//------------------------------------------
		// 削除
		//------------------------------------------




	private:

		DenceTable table_;

		Vector<Type> values_;

	};
}