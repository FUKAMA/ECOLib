#pragma once
#include <atomic>
#include <string_view>
#include <unordered_map>

#if defined(_MSC_VER)
# define TYPE_FUNC_NAME __FUNCSIG__
#else
# define TYPE_FUNC_NAME __PRETTY_FUNCTION__
#endif

namespace utl
{
	// 型ごとに一意に割り振られる連番のIDを表す型エイリアス
	using TypeID = size_t;

	/// <summary>
	///	型の一意なIDを生成、管理するクラス
	/// 共通のインスタンスを使用することで全ての翻訳単位で同じIDを使用することができる
	/// シングルトンインスタンスに実体化する場合は要注意
	/// </summary>
	class TypeIDAllocator
	{
	public:

		/// <summary>
		/// 一意な型IDをコンテナから取得する
		/// まだ生成されてなければ生成してコンテナに追加する
		/// </summary>
		/// <typeparam name="Type"></typeparam>
		/// <returns></returns>
		template<typename...Types>
		TypeID GetID()
		{
			const char* keyName = TYPE_FUNC_NAME;
			auto itr = m_umNameToTypeID.find(keyName);
			if (itr == m_umNameToTypeID.end())
			{
				Allocate(keyName);
				itr = m_umNameToTypeID.find(keyName);
			}
			return itr->second;
		}

	private:

		/// <summary>
		/// 新しく型IDを生成して登録する
		/// </summary>
		/// <param name="a_keyName">型ごとに展開される関数名の文字列</param>
		void Allocate(const char* a_keyName);

	private:

		std::atomic<TypeID> m_counter = 0;

		std::unordered_map<std::string, TypeID> m_umNameToTypeID;
	};

	/// <summary>
	/// コンパイル時にGetTypeID関数内でインスタンスが作成され、
	/// 型ごとに一意なIDを格納するクラス
	/// </summary>
	/// <typeparam name="Type"></typeparam>
	template<typename...Types>
	class TypeIDContainer
	{
	private:
		template<typename...T>
		friend TypeID GetTypeID(TypeIDAllocator& a_allocator);

		TypeIDContainer()
		{

		}
	public:

		/// <summary>
		/// このコンテナが管理する型IDを返す
		/// 型IDが割り振られてなければ、引数のアロケータからIDを受け取る
		/// </summary>
		/// <param name="a_allocator"></param>
		/// <returns></returns>
		inline TypeID GetID(TypeIDAllocator& a_allocator)
		{
			if (m_id == 0)
			{
				m_id = a_allocator.GetID<Types...>();
			}
			return m_id;
		}

		inline TypeID GetID()
		{
			return m_id;
		}

	private:

		TypeID m_id = 0;

	};

	class StaticTypeIDAllocator
	{
	public:
		inline static size_t Allocate()
		{
			return nextGeneID++;
		}

	private:

		inline static size_t nextGeneID = 0;
	};


	template<typename...Types>
	struct StaticTypeIDContainer
	{
		inline static TypeID id = StaticTypeIDAllocator::Allocate();
	};

	/// <summary>
	/// 型ごとに一意で連番なIDを生成する関数
	/// 同じTypeIDAllocatorのインスタンスを使うことで異なる翻訳単位間で共通の型IDを使うことができる
	/// </summary>
	/// <typeparam name="Type"></typeparam>
	/// <param name="a_pAllocator"></param>
	/// <returns></returns>
	template<typename...Types>
	inline static TypeID GetTypeID(TypeIDAllocator& a_allocator)
	{
		static TypeIDContainer<Types...> idContainer;
		return idContainer.GetID(a_allocator);
	}

	/// <summary>
	/// 型ごとに一意で連番なIDを生成する関数
	/// 高速な代わりに異なる静的領域間では同じIDを参照不可
	/// </summary>
	/// <typeparam name="...Types"></typeparam>
	/// <returns></returns>
	template<typename...Types>
	inline static TypeID GetTypeID()
	{
		return StaticTypeIDContainer<Types...>::id;
	}

}