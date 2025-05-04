#pragma once
#include "../../Allocator/MemoryAllocator.h"

namespace utl
{
	/// <summary>
	/// �T�C�Y�̕ύX���\�ȃ������u���b�N
	/// </summary>
	class ResizableMemoryBlock
	{
	public:

		//-----------------------------------------
		// �쐬
		//-----------------------------------------

		/// <summary>
		/// ��̃������u���b�N���쐬����
		/// </summary>
		/// <param name="alloc"></param>
		ResizableMemoryBlock(IMemoryAllocator* alloc = nullptr);

		/// <summary>
		/// �������T�C�Y���w�肵�Ȃ��烁�����u���b�N���쐬����
		/// </summary>
		/// <param name="size"></param>
		/// <param name="alloc"></param>
		ResizableMemoryBlock(const size_t size, IMemoryAllocator* alloc = nullptr);

		//-----------------------------------------
		// ���
		//-----------------------------------------

		/// <summary>
		/// �����̊Ǘ����郁�����̃R�s�[�����̃u���b�N�ɍ쐬����
		/// </summary>
		/// <param name="src"></param>
		ResizableMemoryBlock(const ResizableMemoryBlock& src);

		/// <summary>
		/// �����̊Ǘ����郁�����̃R�s�[�����̃u���b�N�ɍ쐬����
		/// </summary>
		/// <param name="src"></param>
		/// <returns></returns>
		ResizableMemoryBlock& operator=(const ResizableMemoryBlock& src);

		//-----------------------------------------
		// �J��
		//-----------------------------------------

		/// <summary>
		/// �m�ۂ��Ă郁�������J������
		/// </summary>
		void Reset();

		~ResizableMemoryBlock();

		//-----------------------------------------
		// ����
		//-----------------------------------------

		/// <summary>
		/// �g�p���郁�����A���P�[�^��ύX����
		/// </summary>
		/// <param name="alloc"></param>
		void ChangeMemoryAllocator(IMemoryAllocator* alloc);

		/// <summary>
		/// �Ǘ����郁�����̃T�C�Y��ύX����
		/// </summary>
		/// <param name="size"></param>
		/// <returns></returns>
		bool Resize(const size_t capacity);

		/// <summary>
		/// �f�[�^���R�s�[����
		/// </summary>
		/// <param name="begin"></param>
		/// <param name="memSize"></param>
		void Copy(const void* begin, const size_t memSize);

		//-----------------------------------------
		// �擾
		//-----------------------------------------

		/// <summary>
		/// �g�p���Ă郁�����A���P�[�^���擾
		/// </summary>
		/// <returns></returns>
		inline IMemoryAllocator* MemoryAllocator()const
		{
			return alloc_.Get();
		}

		/// <summary>
		/// �m�ۂ����������̐擪�A�h���X���擾
		/// </summary>
		/// <returns></returns>
		inline void* Data()const
		{
			return mem_;
		}

		/// <summary>
		/// �u���b�N�̗e�ʂ��擾
		/// </summary>
		/// <returns></returns>
		inline size_t Capacity()const
		{
			return capacity_;
		}

	private:

		MemoryAllocatorHolder alloc_;

		void* mem_;

		size_t capacity_;
	};
}
