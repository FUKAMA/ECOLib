#pragma once
#include "../Vector/Vector.h"

namespace utl
{
	using SwapKey = uint32_t;
	using SwapIndex = uint32_t;
	constexpr uint32_t NULL_SWAP_INDEX = UINT32_MAX;

	/// <summary>
	/// �A�Ԃ̐������L�[�Ƃ��A�폜����Ƃ��ɍŌ�̗v�f���폜����v�f�ɏ㏑������e�[�u��
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
		// �쐬
		//-----------------------------------

		DenceTable(IMemoryAllocator* alloc = nullptr)
			:linkInfo_(alloc)
			, size_(0)
		{

		}

		//-----------------------------------
		// �J��
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
		// �ǉ�
		//-----------------------------------

		SwapIndex Insert(const SwapKey key)
		{
			// ���łɒǉ��ς݂̃L�[�Ȃ炻�̂܂ܕԂ�
			if (CheckActive(key))
			{
				KeyIndex& info = linkInfo_[key];
				return info.index;
			}

			// �Y���������߂Ȃ���v�f���𑝉�
			SwapIndex index = ++size_;

			// �܂��ǉ����ĂȂ���Βǉ�
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
		// �擾
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
		// �폜
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

			// �폜����v�f�̓Y�����ƍŌ�̗v�f�̓Y�������قȂ�΁A
			if (removeIndex != backIndex)
			{
				// �폜����v�f�ɍŌ�̗v�f�𒣂�t����
				linkInfo_[removeIndex].key = backKey;
				linkInfo_[removeKey].index = backIndex;
			}

			// �Ō�̗v�f�𖳌���
			linkInfo_[backKey].index = NULL_SWAP_INDEX;
			--size_;

			// �ʒu���ړ������\��������Ō�̗v�f�̓Y������Ԃ�
			return backIndex;
		}

	private:

		// KeyToIndex��IndexToKey�����˂�R���e�i
		Vector<KeyIndex> linkInfo_;
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

		//------------------------------------------
		// �폜
		//------------------------------------------




	private:

		DenceTable table_;

		Vector<Type> values_;

	};
}