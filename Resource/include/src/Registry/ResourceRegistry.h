#pragma once
#include "TypeInfo.hpp"

namespace res
{
	/// <summary>
	/// ���\�[�X�̏����i�[���Ă����N���X
	/// �S�Ẵ��\�[�X�X�g���[�W�ŋ��ʂ��Ĉ�̃C���X�^���X���g�p����
	/// </summary>
	class ResourceRegistry
	{
	private:
		class ResourceTag
		{

		};
	public:

		ResourceRegistry();

		template<typename ResType>
		inline utl::TypeID GetTypeID()
		{
			return utl::GetTypeID<ResourceTag, ResType>(idAllocator_);
		}


	private:

		utl::TypeIDAllocator idAllocator_;

	};
}
