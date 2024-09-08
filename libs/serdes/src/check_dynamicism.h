#ifndef CHECK_DYNAMICISM_H_
#define CHECK_DYNAMICISM_H_

#include "concepts.h"

class CheckDynamicism
{
public:
	explicit constexpr CheckDynamicism(bool& isDynamic)
		: m_isDynamic {isDynamic}
	{
		m_isDynamic = false;
	}

	template <class T>
	constexpr void operator()(T& value)
	{}

	template <Vector T>
	constexpr void operator()(T& value)
	{
		m_isDynamic = true;
	}

	template <String T>
	constexpr void operator()(T& value)
	{
		m_isDynamic = true;
	}

private:
	bool& m_isDynamic;
};

#endif  // CHECK_DYNAMICISM_H_
