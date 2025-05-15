#pragma once
#include "../Vector/Vector.h"

namespace utl
{
	using DenceKey = uint32_t;
	using DenceIndex = uint32_t;
	// Null��DenceKey,DenceIndex��\���萔
	constexpr uint32_t NULL_DENCE = UINT32_MAX;

	/// <summary>
	/// �A�Ԃ̐������L�[�Ƃ��A�폜����Ƃ��ɍŌ�̗v�f���폜����v�f�ɏ㏑������e�[�u��
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
		// �쐬
		//-----------------------------------

		DenceTable(IMemoryAllocator* alloc = nullptr)
			:linkInfo_(alloc)
			, size_(0)
		{
			// clang
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

		DenceIndex Insert(const DenceKey key)
		{
			// ���łɒǉ��ς݂̃L�[�Ȃ炻�̂܂ܕԂ�
			if (CheckActive(key))
			{
				KeyIndex& info = linkInfo_[key];
				return info.index;
			}
			if (key == NULL_DENCE)
			{
				return NULL_DENCE;
			}

			// �Y���������߂���ɗv�f���𑝉�
			DenceIndex index = size_++;

			// �܂��ǉ����ĂȂ���Βǉ�
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
		// �擾
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
		// ����������
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
		// �폜
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

			// �폜����v�f�̓Y�����ƍŌ�̗v�f�̓Y�������قȂ�΁A
			if (removeIndex != backIndex)
			{
				// �폜����v�f�ɍŌ�̗v�f�𒣂�t����
				linkInfo_[removeIndex].key = backKey;
				linkInfo_[backKey].index = removeIndex;
			}

			// �폜����v�f�̃L�[�ƓY�����𖳌���
			linkInfo_[removeKey].index = NULL_DENCE;
			linkInfo_[backIndex].key = NULL_DENCE;
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
			values_[remIndex] = values_[backIndex];

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
			return values_.EmplaceBack();
		}



	private:

		DenceTable table_;

		Vector<Type> values_;

	};
}

