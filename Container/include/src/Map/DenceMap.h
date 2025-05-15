#pragma once
#include "../Vector/Vector.h"

namespace utl
{
	using DenceKey = uint32_t;
	using DenceIndex = uint32_t;
	// NullのDenceKey,DenceIndexを表す定数
	constexpr uint32_t NULL_DENCE = UINT32_MAX;

	/// <summary>
	/// 連番の整数をキーとし、削除するときに最後の要素を削除する要素に上書きするテーブル
	/// </summary>
	class DenceTable
	{
		struct KeyIndex
		{
			DenceKey key = NULL_DENCE;
			DenceIndex index = NULL_DENCE;
		};
	public:

		//-----------------------------------
		// 作成
		//-----------------------------------

		DenceTable(IMemoryAllocator* alloc = nullptr)
			:linkInfo_(alloc)
			, size_(0)
		{
			// clang
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

		DenceIndex Insert(const DenceKey key)
		{
			// すでに追加済みのキーならそのまま返す
			if (CheckActive(key))
			{
				KeyIndex& info = linkInfo_[key];
				return info.index;
			}
			if (key == NULL_DENCE)
			{
				return NULL_DENCE;
			}

			// 添え字を求めた後に要素数を増加
			DenceIndex index = size_++;

			// まだ追加してなければ追加
			linkInfo_.Resize(key + 1);
			KeyIndex& keyInfo = linkInfo_[key];
			KeyIndex& indexInfo = linkInfo_[index];
			keyInfo.index = index;
			indexInfo.key = key;

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
			if (!linkInfo_.IsContain(key))
			{
				return false;
			}
			return linkInfo_[key].index != NULL_DENCE;
		}

		DenceIndex GetIndex(const DenceKey key)const
		{
			if (!CheckActive(key))
			{
				return NULL_DENCE;
			}
			return linkInfo_[key].index;
		}

		DenceKey GetKey(const DenceIndex index)const
		{
			if (!linkInfo_.IsContain(index))
			{
				return NULL_DENCE;
			}

			return linkInfo_[index].key;
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
			linkInfo_.FitSlack();
		}

		void Fit()
		{
			linkInfo_.Fit();
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

			const DenceIndex removeIndex = linkInfo_[removeKey].index;

			const DenceIndex backIndex = size_ - 1;
			const DenceKey backKey = linkInfo_[backIndex].key;

			// 削除する要素の添え字と最後の要素の添え字が異なれば、
			if (removeIndex != backIndex)
			{
				// 削除する要素に最後の要素を張り付ける
				linkInfo_[removeIndex].key = backKey;
				linkInfo_[backKey].index = removeIndex;
			}

			// 削除する要素のキーと添え字を無効化
			linkInfo_[removeKey].index = NULL_DENCE;
			linkInfo_[backIndex].key = NULL_DENCE;
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
			values_[remIndex] = values_[backIndex];

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
			return values_.EmplaceBack();
		}



	private:

		DenceTable table_;

		Vector<Type> values_;

	};
}

