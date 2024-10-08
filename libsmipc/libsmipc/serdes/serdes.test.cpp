/* MIT License
 *
 * Copyright (c) 2024 Josef de Joanelli (josef@pixelrift.io)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <libsmipc/serdes/serdes.hpp>

#include <gtest/gtest.h>

#include <algorithm>
#include <cstdint>
#include <cstring>
#include <span>
#include <string>
#include <vector>

enum class MyEnum : uint16_t
{
};

struct MyStruct
{
	uint32_t a {};                        // 4, 4
	float b {};                           // 4, 4
	MyEnum c {};                          // 2, 2
	std::vector<uint8_t> d {1, 2, 3, 4};  // 4, 12
	std::string e {"hello world"};        // 4, 19
										  // 18, 41
};

bool operator==(const MyStruct& a, const MyStruct& b)
{
	return std::tie(a.a, a.b, a.c, a.d, a.e) == std::tie(b.a, b.b, b.c, b.d, b.e);
}

std::vector<std::byte> toBytes(std::vector<int> v)
{
	std::vector<std::byte> bytes {};
	std::for_each(v.begin(), v.end(), [&bytes](int i)
				  { bytes.push_back(std::byte(i)); });
	return bytes;
}

SERIALISABLE(MyStruct)
{
	MEMBER(a);
	MEMBER(b);
	MEMBER(c);
	MEMBER(d);
	MEMBER(e);
}

TEST(serdes, variable_integer_0)
{
	const VariableInteger varInt {std::size_t(0u)};
	const VariableInteger varInt2 {varInt.bytes().data()};

	EXPECT_EQ(varInt.value(), varInt2.value());
	EXPECT_EQ(varInt.size(), 1u);
	EXPECT_EQ(varInt2.size(), 1u);
	EXPECT_EQ(varInt.bytes().size(), varInt.size());
	EXPECT_EQ(varInt2.bytes().size(), varInt2.size());
	ASSERT_EQ(varInt.bytes().size(), varInt2.bytes().size());
	EXPECT_EQ(std::memcmp(varInt.bytes().data(), varInt2.bytes().data(), varInt.bytes().size()), 0u);
}

TEST(serdes, variable_integer_1)
{
	const VariableInteger varInt {0x01};
	const VariableInteger varInt2 {varInt.bytes().data()};

	EXPECT_EQ(varInt.value(), varInt2.value());
	EXPECT_EQ(varInt.size(), 1u);
	EXPECT_EQ(varInt2.size(), 1u);
	EXPECT_EQ(varInt.bytes().size(), varInt.size());
	EXPECT_EQ(varInt2.bytes().size(), varInt2.size());
	ASSERT_EQ(varInt.bytes().size(), varInt2.bytes().size());
	EXPECT_EQ(std::memcmp(varInt.bytes().data(), varInt2.bytes().data(), varInt.bytes().size()), 0u);
}

TEST(serdes, variable_integer_7F)
{
	const VariableInteger varInt {0x7F};
	const VariableInteger varInt2 {varInt.bytes().data()};

	EXPECT_EQ(varInt.value(), varInt2.value());
	EXPECT_EQ(varInt.size(), 1u);
	EXPECT_EQ(varInt2.size(), 1u);
	EXPECT_EQ(varInt.bytes().size(), varInt.size());
	EXPECT_EQ(varInt2.bytes().size(), varInt2.size());
	ASSERT_EQ(varInt.bytes().size(), varInt2.bytes().size());
	EXPECT_EQ(std::memcmp(varInt.bytes().data(), varInt2.bytes().data(), varInt.bytes().size()), 0u);
}

TEST(serdes, variable_integer_F0)
{
	const VariableInteger varInt {0xF0};
	const VariableInteger varInt2 {varInt.bytes().data()};

	EXPECT_EQ(varInt.value(), varInt2.value());
	EXPECT_EQ(varInt.size(), 2u);
	EXPECT_EQ(varInt2.size(), 2u);
	EXPECT_EQ(varInt.bytes().size(), varInt.size());
	EXPECT_EQ(varInt2.bytes().size(), varInt2.size());
	ASSERT_EQ(varInt.bytes().size(), varInt2.bytes().size());
	EXPECT_EQ(std::memcmp(varInt.bytes().data(), varInt2.bytes().data(), varInt.bytes().size()), 0u);
}

TEST(serdes, variable_integer_F1)
{
	const VariableInteger varInt {0xF1};
	const VariableInteger varInt2 {varInt.bytes().data()};

	EXPECT_EQ(varInt.value(), varInt2.value());
	EXPECT_EQ(varInt.size(), 2u);
	EXPECT_EQ(varInt2.size(), 2u);
	EXPECT_EQ(varInt.bytes().size(), varInt.size());
	EXPECT_EQ(varInt2.bytes().size(), varInt2.size());
	ASSERT_EQ(varInt.bytes().size(), varInt2.bytes().size());
	EXPECT_EQ(std::memcmp(varInt.bytes().data(), varInt2.bytes().data(), varInt.bytes().size()), 0u);
}

TEST(serdes, variable_integer_FFFFFFFFFFFFFFFF)
{
	const VariableInteger varInt {0xFFFF'FFFF'FFFF'FFFF};
	const VariableInteger varInt2 {varInt.bytes().data()};

	EXPECT_EQ(varInt.value(), varInt2.value());
	EXPECT_EQ(varInt.size(), 9u);
	EXPECT_EQ(varInt2.size(), 9u);
	EXPECT_EQ(varInt.bytes().size(), varInt.size());
	EXPECT_EQ(varInt2.bytes().size(), varInt2.size());
	ASSERT_EQ(varInt.bytes().size(), varInt2.bytes().size());
	EXPECT_EQ(std::memcmp(varInt.bytes().data(), varInt2.bytes().data(), varInt.bytes().size()), 0u);
}

TEST(serdes, size)
{
	const SerialisableMyStruct x {};

	EXPECT_EQ(x.getMinimumSize(), 12);
	EXPECT_EQ(x.getSize(), 27);
	EXPECT_TRUE(x.isDynamic());
	EXPECT_FALSE(x.isStatic());
}

TEST(serdes, serialise)
{
	const SerialisableMyStruct x {};
	const auto expectedBytes {toBytes({0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 1, 2, 3, 4, 11, 'h', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd'})};
	const std::vector<std::byte> bytes {x.serialise()};

	EXPECT_EQ(bytes, expectedBytes);
}

TEST(serdes, deserialise)
{
	const auto bytes {toBytes({0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 1, 2, 3, 4, 11, 'h', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd'})};
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
