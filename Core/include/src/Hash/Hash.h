#pragma once
#include <cstdint>
#include <cstddef>
#include <type_traits>
#include <bit>

namespace utl
{
	// �����̃n�b�V���l
	inline constexpr uint64_t hashOffset = 0xcbf29ce484222325ULL;
	// 
	inline constexpr uint64_t hashPrime = 0x00000100000001B3ULL;



	/// <summary>
	/// �������̉򂩂�n�b�V���l�����߂�
	/// �R���p�C�����ɃT�C�Y���w��ł���ꍇ�Ɏw�肷��
	/// </summary>
	/// <typeparam name="N"></typeparam>
	/// <param name="data"></param>
	/// <param name="seed"></param>
	/// <returns></returns>
	template<size_t Size>
	constexpr uint64_t HashBytes(const uint8_t(&data)[Size],
		uint64_t seed = hashOffset) noexcept
	{
		for (size_t i = 0; i < Size; ++i)
		{
			seed ^= data[i];
			seed *= hashPrime;
		}
		return seed;
	}
	/// <summary>
	/// �������̉򂩂�n�b�V���l�����߂�
	/// </summary>
	/// <typeparam name="N"></typeparam>
	/// <param name="data"></param>
	/// <param name="seed"></param>
	/// <returns></returns>
	template<size_t Size>
	constexpr uint64_t HashBytes(const uint8_t* data,
		uint64_t seed = hashOffset) noexcept
	{
		for (size_t i = 0; i < Size; ++i)
		{
			seed ^= data[i];
			seed *= hashPrime;
		}
		return seed;
	}
	/// <summary>
	/// �������̉򂩂�n�b�V���l�����߂�
	/// </summary>
	/// <param name="data"></param>
	/// <param name="size"></param>
	/// <param name="seed"></param>
	/// <returns></returns>
	constexpr uint64_t HashBytes(const uint8_t* data, uint64_t size,
		uint64_t seed = hashOffset) noexcept
	{
		for (size_t i = 0; i < size; ++i)
		{
			seed ^= *(data + i);
			seed *= hashPrime;
		}
		return seed;
	}

	/// <summary>
	/// ����̃I�u�W�F�N�g�̌^����
	/// </summary>
	/// <typeparam name="Type"></typeparam>
	/// <param name="src"></param>
	/// <param name="seed"></param>
	/// <returns></returns>
	template<typename Type>
	constexpr uint64_t HashType(Type& src,
		uint64_t seed = hashOffset) noexcept
	{
		uint8_t* data = reinterpret_cast<uint8_t*>(&src);
		return HashBytes<sizeof(Type)>(data, seed);
	}

	/// <summary>
	/// �����񂩂�n�b�V���l�����߂�
	/// </summary>
	/// <typeparam name="N"></typeparam>
	/// <param name="str"></param>
	/// <param name="seed"></param>
	/// <returns></returns>
	template<size_t Size>
	constexpr uint64_t HashStrgin(const char(&str)[Size],
		uint64_t seed = hashOffset) noexcept
	{
		for (size_t i = 0; i < Size - 1; ++i)
		{
			seed ^= static_cast<uint8_t>(str[i]);
			seed *= hashPrime;
		}
		return seed;
	}

	/// <summary>
	/// �����̌^�C���X�^���X����n�b�V���l�����߂�
	/// </summary>
	/// <typeparam name="...Types"></typeparam>
	/// <param name="...src"></param>
	/// <param name="seed"></param>
	/// <returns></returns>
	template<typename...Types>
	constexpr uint64_t HashTypes(Types&...src) noexcept
	{
		uint64_t result = hashOffset;
		((result = HashType<Types>(src, result)), ...);
		return result;
	}
}
