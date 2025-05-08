#pragma once

namespace utl
{
	/// <summary>
	/// ���������m�ۂ����i���O���ɒ񋟂���C���^�[�t�F�[�X�N���X
	/// </summary>
	class IMemoryAllocator
	{
	protected:
		friend class MemoryAllocatorHolder;

		/// <summary>
		/// ��̃��������m�ۂ��鏈�����������鉼�z�֐�
		/// �m�ۂ�����������void*�����ŊJ���ł���K�v������
		/// </summary>
		/// <param name="size"></param>
		/// <returns></returns>
		virtual void* Allocate(const size_t size) = 0;

		/// <summary>
		/// �m�ۂ������������J������֐����������鉼�z�֐�
		/// </summary>
		/// <param name="allocated"></param>
		virtual void Deallocate(void* allocated) = 0;

	};

	/// <summary>
	/// �������A���P�[�^�̎Q�Ƃ��󂯎��Ƃ��ɕ֗��ȃN���X
	/// �������A���P�[�^���w�肳��Ă�΂�����g���A
	/// �w�肳��ĂȂ����C++�W���̎�i���g��
	/// </summary>
	class MemoryAllocatorHolder
	{
	public:
		MemoryAllocatorHolder(IMemoryAllocator* alloc = nullptr)
			:alloc_(alloc)
		{
		}

		MemoryAllocatorHolder(const MemoryAllocatorHolder& src)
			:alloc_(src.alloc_)
		{
		}

		IMemoryAllocator* Get()const
		{
			return alloc_;
		}

		[[nodiscard]] void* Allocate(const size_t size);

		/// <summary>
		/// ����̌^�̃C���X�^���X��C�ӂ̌��i�[�ł���T�C�Y�̋󃁃������m�ۂ���
		/// </summary>
		/// <typeparam name="Type"></typeparam>
		/// <param name="num"></param>
		/// <returns></returns>
		template<typename Type>
		[[nodiscard]] Type* Allocate(const size_t num = 1)
		{
			void* mem = Allocate(sizeof(Type) * num);
			return static_cast<Type*>(mem);
		}
		void Deallocate(void* allocated);

		void Reset(IMemoryAllocator* alloc = nullptr)
		{
			alloc_ = alloc;
		}

	private:

		IMemoryAllocator* alloc_;

	};

	/// <summary>
	/// �������A���P�[�^�̎�������Ɏw�肵�Ȃ��������Ɏg�p���邽�߂̃N���X
	/// C++�W���̎�i�Ń��������m�ہA�J������
	/// </summary>
	class DefaultMemoryAllocator :public IMemoryAllocator
	{
		/// <summary>
		/// malloc�ŋ�̃��������m�ۂ���
		/// </summary>
		/// <param name="size"></param>
		/// <returns></returns>
		virtual void* Allocate(const size_t size)override final;

		/// <summary>
		/// malloc�Ŋm�ۂ�����������free�ŊJ������
		/// </summary>
		/// <param name="allocated"></param>
		virtual void Deallocate(void* allocated)override final;

	};

}
