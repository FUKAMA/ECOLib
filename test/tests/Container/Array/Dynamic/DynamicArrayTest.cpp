#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#include <thread>
#include <vector>

struct ResourceID
{
	uint32_t index = 0;
	uint16_t version = 0;
	uint16_t type = 0;
};



TEST(DynamicArrayTest, TestName)
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	utl::ResizableArray<int> a(5, nullptr, 554);

	a.Get(4);

	std::atomic<int> aaa;

	utl::DynamicArray<int, 5> s(nullptr, 77);

	utl::StaticArray<int, 5> d(7);

	utl::Vector<int> vA;

	//vA.PushBack(1);
	//vA.PushBack(1);
	//vA.PushBack(1);
	//vA.EmplaceBack(1);

	vA.Resize(0);
	vA.Resize(1);
	vA.Resize(2);
	vA.Resize(3);
	vA.Resize(4);
	vA.Resize(5);
	vA.Resize(9, 4);
	vA.Resize(9, 100);



	//utl::Vector<utl::RecyclableIDAllocator::IDInfo> vFF;


	//vFF.Resize(0);
	//vFF.Resize(1);
	//vFF.Resize(2);
	//vFF.Resize(3);
	//vFF.Resize(4);
	//vFF.Resize(5);
	//vFF.Resize(6);

	int& vvvv = vA[3];

	vvvv = 9999;
	int vvvav = vA[3];

	utl::RecyclableIDAllocator allocator;

	auto id1 = allocator.Allocate();
	auto id2 = allocator.Allocate();

	ResourceID rID;
	allocator.Allocate(rID, &ResourceID::index, &ResourceID::version);
	allocator.Allocate(rID, &ResourceID::index, &ResourceID::version);
	allocator.Deallocate(rID.index, rID.version);
	allocator.Allocate(rID, &ResourceID::index, &ResourceID::version);
	allocator.Allocate(rID, &ResourceID::index, &ResourceID::version);
	allocator.Deallocate(rID.index, rID.version);
	allocator.Deallocate(rID.index, rID.version);
	allocator.Allocate(rID, &ResourceID::index, &ResourceID::version);
	allocator.Allocate(rID, &ResourceID::index, &ResourceID::version);
	allocator.Allocate(rID, &ResourceID::index, &ResourceID::version);
	allocator.Deallocate(rID.index, rID.version);
	allocator.Allocate(rID, &ResourceID::index, &ResourceID::version);
	allocator.Deallocate(rID.index, rID.version);
	allocator.Allocate(rID, &ResourceID::index, &ResourceID::version);

	allocator;


	utl::DenceMap<int> dmAAAA;

	dmAAAA[100] = 20;
	dmAAAA[2] = 30;
	dmAAAA[3] = 40;
	dmAAAA[3000] = 50;

	{
		int bbbb = dmAAAA[100];
		int aaaa = dmAAAA[2];
		int cccc = dmAAAA[3];
		int dddd = dmAAAA[3000];
	}
	dmAAAA.Remove(2);
	dmAAAA.Remove(3);
	{
		int bbbb = dmAAAA[100];
		int* aaaa = dmAAAA.Get(2);
		int* cccc = dmAAAA.Get(3);
		int* gf = dmAAAA.Get(3000);
		//int aaaa = dmAAAA[2];
		//int cccc = dmAAAA[3];
		int dddd = dmAAAA[3000];
	}
	dmAAAA.FitSlack();
	{
		int bbbb = dmAAAA[100];
		int* aaaa = dmAAAA.Get(2);
		int* cccc = dmAAAA.Get(3);
		int* gf = dmAAAA.Get(3000);
		//int aaaa = dmAAAA[2];
		//int cccc = dmAAAA[3];
		int dddd = dmAAAA[3000];
	}
	dmAAAA.Fit();
	{
		int bbbb = dmAAAA[100];
		int* aaaa = dmAAAA.Get(2);
		int hhh = dmAAAA[2];
		int* cccc = dmAAAA.Get(3);
		int* gf = dmAAAA.Get(3000);
		//int aaaa = dmAAAA[2];
		//int cccc = dmAAAA[3];
		int dddd = dmAAAA[3000];
	}

	{
		size_t aaaaaaa = 25 / 8;
		utl::BitSet<256, uint8_t> aaa;
		utl::BitSet<8, uint8_t> aaa8;
		size_t size = sizeof(utl::BitSet<87, uint8_t>);
		size = sizeof(utl::BitSet<3, uint8_t>);
		size = sizeof(utl::BitSet<256, uint8_t>);
		size = sizeof(utl::BitSet<256, uint8_t>);
	}
	{
		utl::BitSet<256> aaa1;
		utl::BitSet<256> aaa2;
		utl::BitSet<8> aaa8;
		aaa8.Set(2);

		EXPECT_EQ(true, aaa1 == aaa2);
		EXPECT_EQ(false, aaa1 != aaa2);
		aaa1.Set(2);
		EXPECT_EQ(false, aaa1 == aaa2);
		EXPECT_EQ(true, aaa1 != aaa2);

		if (aaa1 == aaa2)
		{
			int fds = 9;
		}
		if (aaa1 == aaa2)
		{
			int fds = 9;
		}

		size_t size = sizeof(utl::BitSet<87>);
		size = sizeof(utl::BitSet<256>);
		size = sizeof(utl::BitSet<256>);
	}

	utl::ResizableBitSet<256> bs1;
	utl::ResizableBitSet<256> bs2;
	utl::ResizableBitSet<256> bs3;
	EXPECT_EQ(true, bs1 == bs1);
	bs1.Set(10000);
	bs1.Reset(10000);
	EXPECT_EQ(false, bs1.Test(10000));
	bs1.Set(10000);
	EXPECT_EQ(true, bs1.Test(10000));
	EXPECT_EQ(true, bs1.IsContain(bs1));
	EXPECT_EQ(true, bs1.IsContain(bs2));
	EXPECT_EQ(false, bs2.IsContain(bs1));

	utl::ChunkVector<int> hhh(256);

	hhh.Begin();
	hhh.EmplaceBack(123);
	hhh.Begin();
	EXPECT_EQ(123, *hhh.Back());
	for (size_t i = 0; i < 100000; i++)
	{
		hhh.EmplaceBack(123);
	}
	EXPECT_EQ(1 + 100000, hhh.Size());
	hhh.Size();


	hhh.Resize(10);
	hhh.Resize(1000);


	utl::ChunkDenceMap<int> cdmAAA(5);

	cdmAAA.Insert(8000, 50);

}

