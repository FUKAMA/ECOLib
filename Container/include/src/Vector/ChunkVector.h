#pragma once
#include "Vector.h"

namespace utl
{
	/// <summary>
	/// �������̍Ċm�ۂ��`�����N�P�ʂōs�����������A������ϒ��z��
	/// </summary>
	/// <typeparam name="Type"></typeparam>
	template<typename Type>
	class ChunkVector
	{
	public:
		//------------------------------------------
		// �쐬
		//------------------------------------------

		/// <summary>
		/// �`�����N�̃T�C�Y�A�Ǘ��p�̃������A���P�[�^�A�v�f�p�̃������A���P�[�^���w�肵�č쐬
		/// </summary>
		/// <param name="chunkSize"></param>
		/// <param name="alloc"></param>
		/// <param name="valAlloc"></param>
		ChunkVector(const size_t chunkSize, IMemoryAllocator* alloc, IMemoryAllocator* valAlloc)
			: chunks_(alloc)
			, valueMemAlloc_(valAlloc)
			, chunkSize_(chunkSize)
		{

		}
		/// <summary>
		/// �`�����N�̃T�C�Y�ƊǗ��p�A�v�f�p�̋��p�������A���P�[�^���w�肵�č쐬
		/// </summary>
		/// <param name="chunkSize"></param>
		/// <param name="alloc"></param>
		ChunkVector(const size_t chunkSize = 64, IMemoryAllocator* alloc = nullptr)
			: chunks_(alloc)
			, valueMemAlloc_(alloc)
			, chunkSize_(chunkSize)
		{

		}

		/// <summary>
		/// �`�����N�̃T�C�Y�A������Ԃ̗e�ʁA�Ǘ��p�̃������A���P�[�^�A�v�f�p�̃������A���P�[�^���w�肵�č쐬
		/// </summary>
		/// <param name="chunkSize"></param>
		/// <param name="capacity"></param>
		/// <param name="alloc"></param>
		/// <param name="valAlloc"></param>
		ChunkVector(const size_t chunkSize, const size_t capacity, IMemoryAllocator* alloc, IMemoryAllocator* valAlloc)
			: chunks_(GetChunkIndex(capacity) + 1, alloc)
			, valueMemAlloc_(valAlloc)
			, chunkSize_(chunkSize)
		{

		}
		/// <summary>
		/// �`�����N�̃T�C�Y�ƊǗ��p�A������Ԃ̗e�ʁA�v�f�p�̋��p�������A���P�[�^���w�肵�č쐬
		/// </summary>
		/// <param name="chunkSize"></param>
		/// <param name="capacity"></param>
		/// <param name="alloc"></param>
		ChunkVector(const size_t chunkSize, const size_t capacity, IMemoryAllocator* alloc = nullptr)
			: chunks_(GetChunkIndex(capacity) + 1, alloc)
			, valueMemAlloc_(alloc)
			, chunkSize_(chunkSize)
		{

		}

		//------------------------------------------
		// ���
		//------------------------------------------

		/// <summary>
		/// �R�s�[���č쐬����R���X�g���N�^
		/// </summary>
		/// <param name="src"></param>
		ChunkVector(const ChunkVector& src)
			:chunks_(src.chunks_)
			, valueMemAlloc_(src.valueMemAlloc_.Get())
			, chunkSize_(src.chunkSize_)
		{

		}
		/// <summary>
		/// ���łɊm�ۂ����v�f���J�����R�s�[����
		/// </summary>
		/// <param name="src"></param>
		/// <returns></returns>
		ChunkVector& operator=(const ChunkVector& src)
		{
			Clear();
			chunks_ = src.chunks_;
			chunkSize_ = src.chunkSize_;
			valueMemAlloc_ = src.valueMemAlloc_;
			return *this;
		}


		/// <summary>
		/// Src�������č쐬����
		/// </summary>
		/// <param name="src"></param>
		ChunkVector(ChunkVector&& src)
			:chunks_(src.chunks_)
			, valueMemAlloc_(src.valueMemAlloc_.Get())
			, chunkSize_(src.chunkSize_)
		{

		}
		/// <summary>
		/// Src��������
		/// </summary>
		/// <param name="src"></param>
		/// <returns></returns>
		ChunkVector& operator=(ChunkVector&& src)
		{
			Clear();
			chunks_ = src.chunks_;
			chunkSize_ = src.chunkSize_;
			valueMemAlloc_ = src.valueMemAlloc_;
			return *this;
		}

		//------------------------------------------
		// �J��
		//------------------------------------------

		~ChunkVector()
		{
			Clear();
		}

		/// <summary>
		/// �m�ۂ����v�f��S�Ă��J������
		/// </summary>
		void Clear()
		{
			chunks_.Clear();
		}

		/// <summary>
		/// �����̗v�f���폜����
		/// �����̃`�����N����ɂȂ�����`�����N���ƃ|�b�v����
		/// </summary>
		void PopBack()
		{
			if (chunks_.Empty())
			{
				return;
			}
			// �����̃`�����N�̖����̗v�f���|�b�v
			chunks_.Back()->PopBack();
			// �����̃`�����N����ɂȂ����疖���̃`�����N���|�b�v
			if (chunks_.Back()->Empty())
			{
				chunks_.PopBack();
			}
		}

		//------------------------------------------
		// �ǉ�
		//------------------------------------------

		/// <summary>
		/// �Q�Ƃ��疖���ɗv�f��ǉ�����
		/// </summary>
		/// <param name="src"></param>
		/// <returns></returns>
		Type* PushBack(const Type& src)
		{
			// �`�����N��������΍쐬
			if (chunks_.Empty())
			{
				chunks_.EmplaceBack(chunkSize_, valueMemAlloc_.Get());
			}
			// �����̃`�����N�ɋ󂫂��Ȃ���΍쐬
			if (chunks_.Back()->Size() >= chunkSize_)
			{
				chunks_.EmplaceBack(chunkSize_, valueMemAlloc_.Get());
			}
			return chunks_.Back()->PushBack(src);
		}
		/// <summary>
		/// �����ɋ�̗v�f��ǉ�
		/// </summary>
		/// <returns></returns>
		Type* PushBack()
		{
			// �`�����N��������΍쐬
			if (chunks_.Empty())
			{
				chunks_.EmplaceBack(chunkSize_, valueMemAlloc_.Get());
			}
			// �����̃`�����N�ɋ󂫂��Ȃ���΍쐬
			if (chunks_.Back()->Size() >= chunkSize_)
			{
				chunks_.EmplaceBack(chunkSize_, valueMemAlloc_.Get());
			}
			return chunks_.Back()->PushBack();
		}

		/// <summary>
		/// �R���X�g���N�^�������疖���ɗv�f��ǉ�����
		/// </summary>
		/// <typeparam name="...ArgTypes"></typeparam>
		/// <param name="...args"></param>
		/// <returns></returns>
		template<typename...ArgTypes>
		Type* EmplaceBack(ArgTypes&&...args)
		{
			// �`�����N��������΍쐬
			if (chunks_.Empty())
			{
				chunks_.EmplaceBack(chunkSize_, valueMemAlloc_.Get());
			}
			// �����̃`�����N�ɋ󂫂��Ȃ���΍쐬
			if (chunks_.Back()->Size() >= chunkSize_)
			{
				chunks_.EmplaceBack(chunkSize_, valueMemAlloc_.Get());
			}
			return chunks_.Back()->EmplaceBack(args...);
		}

		//------------------------------------------
		// �擾
		//------------------------------------------


		/// <summary>
		/// �����̓Y�����̗v�f������`�����N�̓Y���������߂�
		/// </summary>
		/// <param name="index"></param>
		/// <returns></returns>
		inline constexpr size_t GetChunkIndex(const size_t index)const
		{
			return index / chunkSize_;
		}

		/// <summary>
		/// �����̗v�f���`�����N���Ŋi�[�����Y���������߂�
		/// </summary>
		/// <param name="index"></param>
		/// <returns></returns>
		inline constexpr size_t GetValueIndex(const size_t index)const
		{
			return index % chunkSize_;
		}

		/// <summary>
		/// �`�����N��̑傫�������߂�
		/// </summary>
		/// <returns></returns>
		inline constexpr size_t GetChunkSize()const
		{
			return chunkSize_;
		}

		/// <summary>
		/// �`�����N�̐������߂�
		/// </summary>
		/// <returns></returns>
		inline constexpr size_t GetChunkNum()const
		{
			return chunks_.Size();
		}

		/// <summary>
		/// �`�����N��Y�������g���擾
		/// </summary>
		/// <param name="chunkIndex"></param>
		/// <returns></returns>
		const Vector<Type>* GetChunk(const size_t chunkIndex)const
		{
			return chunks_.Get(chunkIndex);
		}

		/// <summary>
		/// �Y��������v�f�̃|�C���^���擾
		/// </summary>
		/// <param name="index"></param>
		/// <returns></returns>
		Type* Get(const size_t index)const
		{
			const size_t chunkIndex = GetChunkIndex(index);
			const size_t valueIndex = GetValueIndex(index);

			const Vector<Type>* backChunk = GetChunk(chunkIndex);
			assert(backChunk != nullptr && "�͈͊O�A�N�Z�X�ł�");

			Type* ptr = backChunk->Get(valueIndex);
			assert(ptr != nullptr && "�͈͊O�A�N�Z�X�ł�");

			return ptr;
		}

		/// <summary>
		/// �Y��������v�f�̎Q�Ƃ��擾
		/// </summary>
		/// <param name="index"></param>
		/// <returns></returns>
		Type& operator[](const size_t index)const
		{
			return chunks_[GetChunkIndex(index)][GetValueIndex(index)];
		}
		/// <summary>
		/// �Y��������v�f�̎Q�Ƃ��擾
		/// </summary>
		/// <param name="index"></param>
		/// <returns></returns>
		Type& operator[](const size_t index)
		{
			return chunks_[GetChunkIndex(index)][GetValueIndex(index)];
		}

		/// <summary>
		/// �����̃u���b�N�̐擪�A�h���X���擾����
		/// </summary>
		/// <param name="index"></param>
		/// <returns></returns>
		inline constexpr Type* BeginFromChunk(const size_t index = 0)const
		{
			if (!chunks_.IsContain(index))
			{
				return nullptr;
			}
			return chunks_[index].Begin();
		}
		/// <summary>
		/// �擪�̗v�f�̃|�C���^���擾
		/// �擪�̃`�����N�̐擪�̗v�f�̃|�C���^���擾
		/// �`�����N�𒴂�����A���͓r�؂�邽�ߒ���
		/// </summary>
		/// <returns></returns>
		Type* Begin()const
		{
			return BeginFromChunk(0);
		}

		inline constexpr Type* BackFromChunk(const size_t index = 0)const
		{
			if (!chunks_.IsContain(index))
			{
				return nullptr;
			}
			return chunks_[index].Back();
		}
		/// <summary>
		/// �����̗v�f���擾
		/// �����̃`�����N�̖����̗v�f���擾
		/// �`�����N�𒴂�����A���͓r�؂�邽�ߒ���
		/// </summary>
		/// <returns></returns>
		Type* Back()const
		{
			if (chunks_.Empty())
			{
				return nullptr;
			}
			return BackFromChunk(chunks_.Size() - 1);
		}

		/// <summary>
		/// �v�f�̑������擾
		/// </summary>
		/// <returns></returns>
		size_t Size()const
		{
			size_t result = 0;
			if (chunks_.Empty())
			{
				return result;
			}
			if (chunks_.Size() >= 2)
			{
				result += (chunks_.Size() - 1) * chunkSize_;
			}
			result += chunks_.Back()->Size();

			return result;
		}

		/// <summary>
		/// �i�[�\�ȗe�ʂ̑������擾
		/// </summary>
		/// <returns></returns>
		size_t Capacity()const
		{
			return chunks_.Capacity() * chunkSize_;
		}

		/// <summary>
		/// �v�f�������Ė������True
		/// </summary>
		/// <returns></returns>
		bool Empty()const
		{
			if (chunks_.Empty())
			{
				return true;
			}
			return chunks_.Back()->Empty();
		}

		/// <summary>
		/// �����̓Y�������͈͓����Ԃ��֐�
		/// </summary>
		/// <param name="index"></param>
		/// <returns></returns>
		bool IsContain(const size_t index)const
		{
			const size_t chunkIndex = GetChunkIndex(index);
			const size_t valueIndex = GetValueIndex(index);

			if (!chunks_.IsContain(chunkIndex))
			{
				return false;
			}

			return chunks_[chunkIndex].IsContain(valueIndex);
		}

		//------------------------------------------
		// ����������
		//------------------------------------------

		/// <summary>
		/// �v�f�̌���ύX
		/// 
		/// </summary>
		/// <typeparam name="...ArgTypes"></typeparam>
		/// <param name="size"></param>
		/// <param name="...args"></param>
		template<typename...ArgTypes>
		void Resize(const size_t size, ArgTypes&&...args)
		{
			if (size == 0)
			{
				Clear();
				return;
			}

			size_t beforeChunkNum = chunks_.Size();
			size_t afterChunkNum = GetChunkIndex(size - 1) + 1;
			// �`�����N�̐���ύX
			chunks_.Resize(afterChunkNum, chunkSize_, valueMemAlloc_.Get());
			// �`�����N���������Ȃ珉��������
			for (size_t i = beforeChunkNum; i < afterChunkNum; i++)
			{
				chunks_[i].Resize(chunkSize_, args...);
			}

			// �����̃`�����N�̐����������
			chunks_.Back()->Resize(GetValueIndex(size - 1) + 1, args...);
		}

		void Resize(const size_t size, const Type& src)
		{
			if (size == 0)
			{
				Clear();
				return;
			}

			size_t beforeChunkNum = chunks_.Size();
			size_t afterChunkNum = GetChunkIndex(size - 1) + 1;
			// �`�����N�̐���ύX
			chunks_.Resize(afterChunkNum, chunkSize_, valueMemAlloc_.Get());
			// �`�����N���������Ȃ珉��������
			for (size_t i = beforeChunkNum; i < afterChunkNum; i++)
			{
				chunks_[i].Resize(chunkSize_, src);
			}

			// �����̃`�����N�̐����������
			chunks_.Back()->Resize(GetValueIndex(size - 1) + 1, src);
		}



	private:

		Vector<Vector<Type>> chunks_;
		MemoryAllocatorHolder valueMemAlloc_;
		size_t chunkSize_;

	};
}