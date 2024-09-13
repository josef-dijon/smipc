#include "serdes/serdes.h"

#include <cstdint>
#include <gtest/gtest.h>
#include <string>
#include <vector>

enum class MyEnum : uint16_t
{
};

struct MyStruct
{
	uint32_t             a {};               // 4, 4
	float                b {};               // 4, 4
	MyEnum               c {};               // 2, 2
	std::vector<uint8_t> d {1, 2, 3, 4};     // 4, 12
	std::string          e {"hello world"};  // 4, 19
											 // 18, 41
};

SERIALISABLE(MyStruct)
{
	MEMBER(a);
	MEMBER(b);
	MEMBER(c);
	MEMBER(d);
	MEMBER(e);
}

TEST(serdes, test)
{
	const SerialisableMyStruct x {};

	EXPECT_EQ(x.getMinimumSize(), 18);
	EXPECT_EQ(x.getActualSize(), 41);
	EXPECT_TRUE(x.isDynamic());
	EXPECT_FALSE(x.isStatic());
}

int main(int argc, char** argv)
{
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
