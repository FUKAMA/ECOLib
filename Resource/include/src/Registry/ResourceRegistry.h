#pragma once
#include "TypeInfo.hpp"

namespace res
{
	/// <summary>
	/// リソースの情報を格納しておくクラス
	/// 全てのリソースストレージで共通して一つのインスタンスを使用する
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
