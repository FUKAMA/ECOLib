#pragma once
#include "../Vector/Vector.h"
#include <utility>

namespace utl
{
	/// <summary>
	/// �܂΂�ȃL�[
	/// </summary>
	using SparseKey = uint32_t;

	/// <summary>
	/// ���W�����Y����
	/// </summary>
	using DenceIndex = uint32_t;

	// Null��DenceKey,DenceIndex��\���萔
	constexpr uint32_t NULL_VALUE = UINT32_MAX;

	/// <summary>
	/// �A�Ԃ̐������L�[�Ƃ��A�폜����Ƃ��ɍŌ�̗v�f���폜����v�f�ɏ㏑������e�[�u��
	/// </summary>
	class SparseSet
	{
	public:

		//-----------------------------------
		// �쐬
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
		// �J��
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
		// �ǉ�
		//-----------------------------------

		DenceIndex Insert(const SparseKey key)
		{
			// ���łɒǉ��ς݂̃L�[�Ȃ炻�̂܂ܕԂ�
			if (CheckActive(key))
			{
				return keyToIndex_[key];
			}

			// �Y���������߂���ɗv�f���𑝉�
			DenceIndex index = size_++;

			// �܂��ǉ����ĂȂ���Βǉ�
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
		// �擾
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
		// ����������
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
		// �폜
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

			// �폜����v�f�̓Y�����ƍŌ�̗v�f�̓Y�������قȂ�΁A
			if (removeIndex != backIndex)
			{
				// �폜����v�f�ɍŌ�̗v�f�𒣂�t����
				indexToKey_[removeIndex] = backKey;
				keyToIndex_[backKey] = removeIndex;
			}

			// �폜����v�f�̃L�[�ƓY�����𖳌���
			keyToIndex_[removeKey] = NULL_VALUE;
			indexToKey_[backIndex] = NULL_VALUE;
			--size_;

			// �ʒu���ړ������\��������Ō�̗v�f�̓Y������Ԃ�
			return backIndex;
		}

	private:

		Vector<DenceIndex> keyToIndex_;
		Vector<SparseKey> indexToKey_;
		// �o�^�����L�[�̐�
		size_t size_;

	};


}