#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>

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
		int*aaaa = dmAAAA.Get(2);
		int*cccc = dmAAAA.Get(3);
		int*gf = dmAAAA.Get(3000);
		//int aaaa = dmAAAA[2];
		//int cccc = dmAAAA[3];
		int dddd = dmAAAA[3000];
	}
	dmAAAA.FitSlack();
	{
		int bbbb = dmAAAA[100];
		int*aaaa = dmAAAA.Get(2);
		int*cccc = dmAAAA.Get(3);
		int*gf = dmAAAA.Get(3000);
		//int aaaa = dmAAAA[2];
		//int cccc = dmAAAA[3];
		int dddd = dmAAAA[3000];
	}
	dmAAAA.Fit();
	{
		int bbbb = dmAAAA[100];
		int*aaaa = dmAAAA.Get(2);
		int hhh = dmAAAA[2];
		int*cccc = dmAAAA.Get(3);
		int*gf = dmAAAA.Get(3000);
		//int aaaa = dmAAAA[2];
		//int cccc = dmAAAA[3];
		int dddd = dmAAAA[3000];
	}

}

