#pragma once
#include "TypeInfo.hpp"

namespace ecs
{
	using CompID = utl::TypeID;

	class CompTypeIDGenerator
	{
		struct CompTypeTag
		{

		};
	public:

		template<typename CompType>
		inline CompID GetCompID()
		{
			return utl::GetTypeID<CompTypeTag, CompType>();
		}

	private:

		utl::TypeIDAllocator idAllocator;

	};
}