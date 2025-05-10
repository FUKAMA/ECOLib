#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>

TEST(DynamicArrayTest, TestName)
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	utl::ResizableArray<int> a(5, nullptr, 554);

	a.Get(4);

	utl::DynamicArray<int, 5> s(nullptr, 77);

	utl::StaticArray<int, 5> d(7);

	utl::Vector<int> vA;

	vA.PushBack(1);
	vA.PushBack(1);
	vA.PushBack(1);
	vA.EmplaceBack(1);

	vA.Resize(9, 14);
	vA.Resize(9, 4);

	int& vvvv = vA[3];

	vvvv = 9999;
	int vvvav = vA[3];

}

