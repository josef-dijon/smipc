#include "serdes.h"

#include <cstdint>
#include <iostream>
#include <string>

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

int main()
{
	const SerialisableMyStruct x;
	std::cout << "Min size   : " << x.getMinimumSize() << "\n";
	std::cout << "Actual size: " << x.getActualSize() << "\n";
	std::cout << "Dynamic    : " << x.isDynamic() << "\n";
	std::cout << "Static     : " << x.isStatic() << "\n";

	return 0;
}
