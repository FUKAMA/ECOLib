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
		// �ǉ�
		//------------------------------------------


		Type* Insert(const SparseKey key, const Type& src)
		{
			DenceIndex index = NULL_VALUE;

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
		Type* Insert(const SparseKey key, ArgTypes&&...args)
		{
			DenceIndex index = NULL_VALUE;

			// ���łɃL�[���ǉ�����Ă�����㏑������
			if (table_.CheckActive(key))
			{
				index = table_[key];
				values_[index] = Type(args...);
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

		void Remove(const SparseKey removeKey)
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
			// �v�f���R���e�i�ɒǉ�
			// �L���ȗv�f�͑O�l�߂Ŕz�u����邽�߁A�����ɒǉ�����΂���
			return *values_.EmplaceBack();
		}

	private:

		SparseSet table_;
		ChunkVector<Type> values_;

	};
}