#pragma once
#include <cinttypes>
#include "Container.hpp"

namespace utl
{
	class RecyclableIDAllocator
	{
	private:
	public:
		struct IDInfo
		{
			uint32_t version = 0;
			bool activate = false;
		};

		struct AllocateResult
		{
			uint32_t index = 0;
			uint32_t version = 0;
		};

	public:


		RecyclableIDAllocator()
			:nextGeneIndex_(0)
		{

		}

		AllocateResult Allocate()
		{
			AllocateResult result;
			Allocate(result, &AllocateResult::index, &AllocateResult::version);
			return result;
		}

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

			// �o�[�W�������X�V����
			IDInfo& info = indexToInfo_[index];
			version = ++info.version;


			result.*indexPtr = static_cast<IndexType>(index);
			result.*versionPtr = static_cast<VersionType>(version);


		}

		void Deallocate(const uint32_t index, const uint32_t version)
		{

		}

		bool CheckActive(const uint32_t index, const uint32_t version)
		{
			if (indexToInfo_.Size())
			{

			}
			return indexToInfo_[index].activate;
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
