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
	// �^���ƂɈ�ӂɊ���U����A�Ԃ�ID��\���^�G�C���A�X
	using TypeID = size_t;

	/// <summary>
	///	�^�̈�ӂ�ID�𐶐��A�Ǘ�����N���X
	/// ���ʂ̃C���X�^���X���g�p���邱�ƂőS�Ă̖|��P�ʂœ���ID���g�p���邱�Ƃ��ł���
	/// �V���O���g���C���X�^���X�Ɏ��̉�����ꍇ�͗v����
	/// </summary>
	class TypeIDAllocator
	{
	public:

		/// <summary>
		/// ��ӂȌ^ID���R���e�i����擾����
		/// �܂���������ĂȂ���ΐ������ăR���e�i�ɒǉ�����
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
		/// �V�����^ID�𐶐����ēo�^����
		/// </summary>
		/// <param name="a_keyName">�^���ƂɓW�J�����֐����̕�����</param>
		void Allocate(const char* a_keyName);

	private:

		std::atomic<TypeID> m_counter = 0;

		std::unordered_map<std::string, TypeID> m_umNameToTypeID;
	};

	/// <summary>
	/// �R���p�C������GetTypeID�֐����ŃC���X�^���X���쐬����A
	/// �^���ƂɈ�ӂ�ID���i�[����N���X
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
		/// ���̃R���e�i���Ǘ�����^ID��Ԃ�
		/// �^ID������U���ĂȂ���΁A�����̃A���P�[�^����ID���󂯎��
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
	/// �^���ƂɈ�ӂŘA�Ԃ�ID�𐶐�����֐�
	/// ����TypeIDAllocator�̃C���X�^���X���g�����ƂňقȂ�|��P�ʊԂŋ��ʂ̌^ID���g�����Ƃ��ł���
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
	/// �^���ƂɈ�ӂŘA�Ԃ�ID�𐶐�����֐�
	/// �����ȑ���ɈقȂ�ÓI�̈�Ԃł͓���ID���Q�ƕs��
	/// </summary>
	/// <typeparam name="...Types"></typeparam>
	/// <returns></returns>
	template<typename...Types>
	inline static TypeID GetTypeID()
	{
		return StaticTypeIDContainer<Types...>::id;
	}

}