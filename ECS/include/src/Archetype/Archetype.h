#pragma once
#include "Container.hpp"
#include "TypeInfo.hpp"
#include "../Component/Type/CompTypeIDGenerator.h"

namespace ecs
{
	using ArcheBits = utl::ResizableBitSet<64>;

	class Archetype
	{
	public:

		/// <summary>
		/// 
		/// </summary>
		/// <param name="index"></param>
		/// <param name="bits"></param>
		void SetBits(const size_t index, const ArcheBits& bits)
		{
			if (!bits_.IsContain(index))
			{
				bits_.Resize(index + 1);
			}
			bits_[index] = bits;
		}



	private:

		utl::Vector<ArcheBits> bits_;

	};
}
