#include "UniquePtrTest.h"
#include <string>

TEST(UniquePtrTest, TestName)
{
	utl::UniquePtr<int> upAAA;

	upAAA = utl::MakeUnique<int>(5);

	EXPECT_EQ(5, *upAAA);

	utl::UniquePtr<std::string> upString = utl::MakeUnique<std::string>("dfdfdfdf");
	 upString = utl::MakeUnique<std::string>("ff");

	//upString->c_str();
}

