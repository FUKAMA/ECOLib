#include "ResizableMemoryBlock.h"
#include <memory>

namespace utl
{
	ResizableMemoryBlock::ResizableMemoryBlock(IMemoryAllocator* alloc)
		: alloc_(alloc), mem_(nullptr), capacity_(0)
	{

	}

	ResizableMemoryBlock::ResizableMemoryBlock(const size_t size, IMemoryAllocator* alloc)
		: alloc_(alloc), mem_(nullptr), capacity_(0)
	{
		Resize(size);
	}

	ResizableMemoryBlock::ResizableMemoryBlock(const ResizableMemoryBlock& src)
		: alloc_(src.alloc_), mem_(nullptr), capacity_(0)
	{
		*this = src;
	}

	ResizableMemoryBlock& ResizableMemoryBlock::operator=(const ResizableMemoryBlock& src)
	{
		if (this == &src)
		{
			return *this;
		}

		Copy(src.Data(), src.Capacity());

		return *this;
	}

	void ResizableMemoryBlock::Reset()
	{
		if (mem_ == nullptr)
		{
			return;
		}
		alloc_.Deallocate(mem_);
		mem_ = nullptr;
		capacity_ = 0;
	}

	ResizableMemoryBlock::~ResizableMemoryBlock()
	{
		Reset();
	}

	void ResizableMemoryBlock::ChangeMemoryAllocator(IMemoryAllocator* alloc)
	{
		// ���g���Ă�A���P�[�^�Ɠ������̂��w�肵�Ă��牽�����Ȃ�
		if (alloc == alloc_.Get())
		{
			return;
		}

		// �V�������������m�ۂ���
		MemoryAllocatorHolder holder(alloc);
		void* newMem = holder.Allocate(capacity_);
		// �V�����������ɍ��̃������̓��e���ڂ�
		memcpy(newMem, mem_, capacity_);

		// ���̃��������J�����āA�V�����������ƃA���P�[�^���Z�b�g����
		alloc_.Deallocate(mem_);
		mem_ = newMem;
		alloc_.Reset(alloc);
	}

	bool ResizableMemoryBlock::Resize(const size_t capacity)
	{
		// �T�C�Y�̑傫���ɕς�肪�Ȃ���Ή������Ȃ�
		if (capacity_ == capacity)
		{
			return false;
		}
		void* newMem = nullptr;

		// �ύX�O�ƕύX��ŏ������ق��̃T�C�Y���ړ����郁�����̃T�C�Y
		size_t moveMemSize = capacity;
		if (capacity > capacity_)
		{
			moveMemSize = capacity_;
		}

		if (capacity > 0)
		{
			// �V�����g�p���郁�������m��
			newMem = alloc_.Allocate(capacity);
			if (newMem == nullptr)
			{
				return false;
			}
		}

		bool isMove = moveMemSize != 0;
		if (isMove)
		{
			if (newMem == nullptr)
			{
				return false;
			}
			memcpy(newMem, mem_, moveMemSize);
		}

		alloc_.Deallocate(mem_);
		mem_ = newMem;
		capacity_ = capacity;

		return true;
	}

	void ResizableMemoryBlock::Copy(const void* begin, const size_t memSize)
	{
		// ���Ǘ����Ă郁�������J������
		Reset();

		// �Ǘ����Ă郁�����Ɠ������������R�s�[���̃���������Ȃ牽�����Ȃ�
		bool isEqual = begin == mem_ && capacity_ == memSize;
		bool isNonMem = memSize <= 0;
		if (isEqual || isNonMem)
		{
			return;
		}

		Resize(memSize);

		memcpy(mem_, begin, memSize);


	}
}
