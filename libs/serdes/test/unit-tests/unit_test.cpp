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

bool operator==(const MyStruct& a, const MyStruct& b)
{
	return std::tie(a.a, a.b, a.c, a.d, a.e) == std::tie(b.a, b.b, b.c, b.d, b.e);
}

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

	EXPECT_EQ(x.getMinimumSize(), 12);
	EXPECT_EQ(x.getActualSize(), 27);
	EXPECT_TRUE(x.isDynamic());
	EXPECT_FALSE(x.isStatic());
}

TEST(serdes, serialise)
{
	const SerialisableMyStruct   x {};
	const std::vector<std::byte> expectedBytes {
		std::byte(0),
		std::byte(0),
		std::byte(0),
		std::byte(0),
		std::byte(0),
		std::byte(0),
		std::byte(0),
		std::byte(0),
		std::byte(0),
		std::byte(0),
		std::byte(4),
		std::byte(1),
		std::byte(2),
		std::byte(3),
		std::byte(4),
		std::byte(11),
		std::byte('h'),
		std::byte('e'),
		std::byte('l'),
		std::byte('l'),
		std::byte('o'),
		std::byte(' '),
		std::byte('w'),
		std::byte('o'),
		std::byte('r'),
		std::byte('l'),
		std::byte('d')};
	const auto bytes {x.serialise()};

	EXPECT_EQ(bytes, expectedBytes);
}

TEST(serdes, deserialise)
{
	const std::vector<std::byte> bytes {
		std::byte(0),
		std::byte(0),
		std::byte(0),
		std::byte(0),
		std::byte(0),
		std::byte(0),
		std::byte(0),
		std::byte(0),
		std::byte(0),
		std::byte(0),
		std::byte(4),
		std::byte(1),
		std::byte(2),
		std::byte(3),
		std::byte(4),
		std::byte(11),
		std::byte('h'),
		std::byte('e'),
		std::byte('l'),
		std::byte('l'),
		std::byte('o'),
		std::byte(' '),
		std::byte('w'),
		std::byte('o'),
		std::byte('r'),
		std::byte('l'),
		std::byte('d')};

	SerialisableMyStruct x {};
	x.deserialise(bytes);

	const SerialisableMyStruct expected {};

	EXPECT_EQ(*x, *expected);
}

int main(int argc, char** argv)
{
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
