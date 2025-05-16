#pragma once
#include <cinttypes>
#include "Container.hpp"

namespace utl
{
	struct RecyclableID
	{
		uint32_t index = 0;
		uint32_t version = 0;
	};

	class RecyclableIDAllocator
	{
	private:
		struct IDInfo
		{
			uint32_t version = 0;
			bool activate = false;
		};

	public:


		RecyclableIDAllocator()
			:nextGeneIndex_(0)
		{
		}

		RecyclableID Allocate()
		{
			RecyclableID result;
			Allocate(result, &RecyclableID::index, &RecyclableID::version);
			return result;
		}

		/// <summary>
		/// �ė��p�\��ID�𐶐����A�C�ӂ̌^�̔C�ӂ̕ϐ��ɃZ�b�g����
		/// </summary>
		/// <typeparam name="IDType"></typeparam>
		/// <typeparam name="IndexType"></typeparam>
		/// <typeparam name="VersionType"></typeparam>
		/// <param name="result"></param>
		/// <param name="indexPtr"></param>
		/// <param name="versionPtr"></param>
		template<typename IDType, typename IndexType, typename VersionType>
		void Allocate(IDType& result, IndexType IDType::* indexPtr, VersionType IDType::* versionPtr)
		{
			uint32_t index = 0;
			uint32_t version = 0;

			// ���T�C�N���҂��̓Y�������Ȃ���ΐV�����������ďI��
			if (recycleWaitIndeces_.Empty())
			{
				index = ++nextGeneIndex_;
				indexToInfo_.Resize(index + 1);
			}
			else
			{
				index = *recycleWaitIndeces_.Back();
				recycleWaitIndeces_.Resize(recycleWaitIndeces_.Size() - 1);
			}

			// �Y�����̏����X�V����
			IDInfo& info = indexToInfo_[index];
			info.activate = true;
			version = ++info.version;

			result.*indexPtr = static_cast<IndexType>(index);
			result.*versionPtr = static_cast<VersionType>(version);
		}

		void Deallocate(const RecyclableID& id)
		{
			Deallocate(id.index, id.version);
		}

		void Deallocate(const uint32_t index, const uint32_t version)
		{
			// ���łɖ�����ID�Ȃ�Ȃɂ����Ȃ�
			if (!CheckActive(index, version))
			{
				return;
			}

			// �Y�����𖳌��ɂ��A�Y���������T�C�N���҂����X�g�ɒǉ�
			IDInfo& info = indexToInfo_[index];
			info.activate = false;
			recycleWaitIndeces_.EmplaceBack(index);

		}

		bool CheckActive(const uint32_t index, const uint32_t version)
		{
			// �܂���������ĂȂ��Y�����Ȃ疳��
			if (indexToInfo_.Size() <= index)
			{
				return false;
			}

			// �Y�����̍ŐV�o�[�W�����Ŗ�����Ζ���
			IDInfo& info = indexToInfo_[index];
			if (info.version != version)
			{
				return false;
			}

			// �Y�����̗L����Ԃ�Ԃ�
			return info.activate;
		}

	private:

		// ���ɐ�������Y����
		uint32_t nextGeneIndex_;

		// �Y�����ƗL����Ԃ�R�Â��ĊǗ�����R���e�i
		Vector<IDInfo> indexToInfo_;
		// ���T�C�N���҂��̓Y�������i�[����R���e�i
		Vector<uint32_t> recycleWaitIndeces_;

	};

}
