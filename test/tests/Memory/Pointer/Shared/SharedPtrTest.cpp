#include "SharedPtrTest.h"
#include <string>

TEST(SharedPtrTest, TestName)
{
	utl::SharedPtr<int> spAAA;

	spAAA = utl::MakeShared<int>(5);

	EXPECT_EQ(5, *spAAA);

	utl::SharedPtr<std::string> spString1 = utl::MakeShared<std::string>("dfdfdfdf");
	utl::SharedPtr<std::string> spString2 = utl::MakeShared<std::string>("gf");
	spString1 = utl::MakeShared<std::string>("ff");

	spString2 = spString1;
}

