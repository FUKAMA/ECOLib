#pragma once
#include <cstdint>
#include <cstddef>
#include <type_traits>
#include <bit>

namespace utl
{
	// 初期のハッシュ値
	inline constexpr uint64_t hashOffset = 0xcbf29ce484222325ULL;
	// 
	inline constexpr uint64_t hashPrime = 0x00000100000001B3ULL;



	/// <summary>
	/// メモリの塊からハッシュ値を求める
	/// コンパイル時にサイズを指定できる場合に指定する
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
	/// メモリの塊からハッシュ値を求める
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
	/// メモリの塊からハッシュ値を求める
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
	/// 特定のオブジェクトの型から
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
	/// 文字列からハッシュ値を求める
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
	/// 複数の型インスタンスからハッシュ値を求める
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
