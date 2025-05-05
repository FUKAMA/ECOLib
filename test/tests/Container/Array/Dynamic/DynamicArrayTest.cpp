
TEST(DynamicArrayTest, TestName)
{
	utl::ResizableArray<int> a(5, nullptr, 554);

	a.Get(4);

	utl::DynamicArray<int, 5> s;

	utl::StaticArray<int, 5> d(7);
}

