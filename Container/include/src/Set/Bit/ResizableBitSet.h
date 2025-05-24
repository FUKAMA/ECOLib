#pragma once
#include "../../Vector/Vector.h"
#include "BitSet.h"

namespace utl
{
	template<size_t BLOCK_BIT_NUM, typename BlockType = uint64_t>
	class ResizableBitSet
	{
	public:

		//-----------------------------------------
		// 作成
		//-----------------------------------------

		ResizableBitSet()
			:bitSets_()
		{

		}

		//-----------------------------------------
		// 代入
		//-----------------------------------------

		ResizableBitSet(const ResizableBitSet& src)
			:bitSets_(src)
		{

		}
		ResizableBitSet& operator=(const ResizableBitSet& src)
		{
			bitSets_ = src.bitSets_;
			return *this;
		}

		ResizableBitSet(ResizableBitSet&& src)
			:bitSets_(src.bitSets_)
		{

		}
		ResizableBitSet& operator=(ResizableBitSet&& src)
		{
			bitSets_ = src.bitSets_;
			return *this;
		}

		//-----------------------------------------
		// 操作
		//-----------------------------------------


		constexpr void Set(const size_t index)
		{
			const size_t blockIndex = index / BLOCK_BIT_NUM;
			const size_t bitIndex = index % BLOCK_BIT_NUM;

			if (!bitSets_.IsContain(blockIndex))
			{
				bitSets_.Resize(blockIndex + 1);
			}

			bitSets_[blockIndex].Set(bitIndex);
		}


		constexpr void Reset(size_t index)noexcept
		{
			const size_t blockIndex = index / BLOCK_BIT_NUM;
			const size_t bitIndex = index % BLOCK_BIT_NUM;

			if (!bitSets_.IsContain(blockIndex))
			{
				return;
			}
			bitSets_[blockIndex].Reset(bitIndex);
		}

		constexpr void Clear()noexcept
		{
			bitSets_.Clear();
		}

		//-----------------------------------------
		// 比較
		//-----------------------------------------


		[[nodiscard]] constexpr bool Test(size_t index) const noexcept
		{
			const size_t blockIndex = index / BLOCK_BIT_NUM;
			const size_t bitIndex = index % BLOCK_BIT_NUM;
			if (!bitSets_.IsContain(blockIndex))
			{
				return false;
			}
			return bitSets_[blockIndex].Test(bitIndex);
		}

		[[nodiscard]] constexpr bool operator==(const ResizableBitSet<BLOCK_BIT_NUM, BlockType>& src) const noexcept
		{
			const size_t checkNum = bitSets_.Size() < src.bitSets_.Size() ? bitSets_.Size() : src.bitSets_.Size();

			for (size_t i = 0; i < checkNum; ++i)
			{
				if (bitSets_[i] != src.bitSets_[i])
				{
					return false;
				}
			}

			for (size_t i = checkNum; i < bitSets_.Size(); ++i)
			{
				if (!bitSets_[i].Empty())
				{
					return false;
				}
			}

			for (size_t i = checkNum; i < src.bitSets_.Size(); ++i)
			{
				if (!src.bitSets_[i].Empty())
				{
					return false;
				}
			}
			return true;
		}
		[[nodiscard]] constexpr bool operator!=(const ResizableBitSet<BLOCK_BIT_NUM, BlockType>& src) const noexcept
		{
			return !(*this == src);
		}


		/// <summary>
		/// 引数のビットセットを含むならTrue
		/// </summary>
		/// <param name="src"></param>
		/// <returns></returns>
		[[nodiscard]] constexpr bool IsContain(const ResizableBitSet<BLOCK_BIT_NUM, BlockType>& src) const noexcept
		{
			const size_t checkNum = bitSets_.Size() < src.bitSets_.Size() ? bitSets_.Size() : src.bitSets_.Size();

			for (size_t i = 0; i < checkNum; ++i)
			{
				if (!bitSets_[i].IsContain(src.bitSets_[i]))
				{
					return false;
				}
			}

			for (size_t i = checkNum; i < src.bitSets_.Size(); ++i)
			{
				if (!src.bitSets_[i].Empty())
				{
					return false;
				}
			}
			return true;
		}



	private:

		Vector<BitSet<BLOCK_BIT_NUM, BlockType>> bitSets_;

	};
}