#pragma once
#include "../Vector/Vector.h"
#include <utility>

namespace utl
{
	using DenceKey = uint32_t;
	using DenceIndex = uint32_t;
	// Null��DenceKey,DenceIndex��\���萔
	constexpr uint32_t NULL_DENCE = UINT32_MAX;

	/// <summary>
	/// �A�Ԃ̐������L�[�Ƃ��A�폜����Ƃ��ɍŌ�̗v�f���폜����v�f�ɏ㏑������e�[�u��
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
		// �쐬
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
		// �J��
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
		// �ǉ�
		//-----------------------------------

		DenceIndex Insert(const DenceKey key)
		{
			// ���łɒǉ��ς݂̃L�[�Ȃ炻�̂܂ܕԂ�
			if (CheckActive(key))
			{
				return keyToIndex_[key];
				//KeyIndex& info = denceSparce_[key];
				//return info.index;
			}

			// �Y���������߂���ɗv�f���𑝉�
			DenceIndex index = size_++;

			// �܂��ǉ����ĂȂ���Βǉ�
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
		// �擾
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
		// ����������
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
		// �폜
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

			// �폜����v�f�̓Y�����ƍŌ�̗v�f�̓Y�������قȂ�΁A
			if (removeIndex != backIndex)
			{
				// �폜����v�f�ɍŌ�̗v�f�𒣂�t����
				indexToKey_[removeIndex] = backKey;
				keyToIndex_[backKey] = removeIndex;
				//denceSparce_[removeIndex].key = backKey;
				//denceSparce_[backKey].index = removeIndex;
			}

			// �폜����v�f�̃L�[�ƓY�����𖳌���
			keyToIndex_[removeKey] = NULL_DENCE;
			indexToKey_[backIndex] = NULL_DENCE;
			//denceSparce_[removeKey].index = NULL_DENCE;
			//denceSparce_[backIndex].key = NULL_DENCE;
			--size_;

			// �ʒu���ړ������\��������Ō�̗v�f�̓Y������Ԃ�
			return backIndex;
		}

	private:

		// KeyToIndex��IndexToKey�����˂�R���e�i
		//Vector<KeyIndex> denceSparce_;
		Vector<DenceIndex> keyToIndex_;
		Vector<DenceKey> indexToKey_;
		// �o�^�����L�[�̐�
		size_t size_;

	};

	template<typename Type>
	class DenceMap
	{
	public:
		//------------------------------------------
		// �쐬
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
		// �ǉ�
		//------------------------------------------


		Type* Insert(const DenceKey key, const Type& src)
		{
			DenceIndex index = NULL_DENCE;

			// ���łɃL�[���ǉ�����Ă�����㏑������
			if (table_.CheckActive(key))
			{
				index = table_[key];
				values_[index] = src;
				return &values_[index];
			}

			index = table_.Insert(key);
			// �v�f���R���e�i�ɒǉ�
			// �L���ȗv�f�͑O�l�߂Ŕz�u����邽�߁A�����ɒǉ�����΂���
			return values_.EmplaceBack(src);
		}

		template<typename...ArgTypes>
		Type* Insert(const DenceKey key, ArgTypes&&...args)
		{
			DenceIndex index = NULL_DENCE;

			// ���łɃL�[���ǉ�����Ă�����㏑������
			if (table_.CheckActive(key))
			{
				index = table_[key];
				//values_[index] = Type(args...);
				return &values_[index];
			}

			index = table_.Insert(key);
			// �v�f���R���e�i�ɒǉ�
			// �L���ȗv�f�͑O�l�߂Ŕz�u����邽�߁A�����ɒǉ�����΂���
			return values_.EmplaceBack(args...);
		}

		//------------------------------------------
		// �폜
		//------------------------------------------

		void Remove(const DenceKey removeKey)
		{
			// ���łɖ����Ȃ牽�����Ȃ�
			if (!table_.CheckActive(removeKey))
			{
				return;
			}

			DenceIndex remIndex = table_.GetIndex(removeKey);

			// �e�[�u������L�[���폜���A�ꏊ������ւ�����L�[�ƓY�������擾
			DenceIndex backIndex = table_.Remove(removeKey);

			// �폜����v�f�̖����̗v�f���㏑��
			if (remIndex != backIndex)
			{
				values_[remIndex] = std::move(values_[backIndex]);
			}

			// �����̗v�f�𖳌���
			values_.PopBack();
		}

		void Clear()
		{
			table_.Reset();
			values_.Clear();
		}

		//------------------------------------------
		// ����������
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
		// �擾
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
			// �v�f���R���e�i�ɒǉ�
			// �L���ȗv�f�͑O�l�߂Ŕz�u����邽�߁A�����ɒǉ�����΂���
			return *values_.EmplaceBack();
		}



	private:

		SparseSet table_;

		Vector<Type> values_;

	};
}

