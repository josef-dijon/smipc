#ifndef SERIALISABLE_H_
#define SERIALISABLE_H_

#include "calculate_actual_size.h"
#include "calculate_minimum_size.h"
#include "check_dynamicism.h"
#include "operator.h"

#include <cstdint>

template <class T>
class Serialisable
{
public:
	constexpr void serialiseInto(std::vector<std::byte>& data) const
	{
		processMembers(Operator(Serialise(data)));
	}

	constexpr auto serialise() const -> std::vector<std::byte>
	{
		std::vector<std::byte> data {};
		data.reserve(getActualSize());
		processMembers(Operator(Serialise(data)));
		return data;
	}

	constexpr auto getMinimumSize() const -> std::size_t
	{
		std::size_t size {};
		processMembers(Operator(CalculateMinimumSize(size)));
		return size;
	}

	constexpr auto getActualSize() const -> std::size_t
	{
		std::size_t size {};
		processMembers(Operator(CalculateActualSize(size)));
		return size;
	}

	constexpr auto isDynamic() const -> bool
	{
		bool dynamic {false};
		processMembers(Operator(CheckDynamicism(dynamic)));
		return dynamic;
	}

	constexpr auto isStatic() const -> bool
	{
		return ! isDynamic();
	}

	constexpr T& operator*()
	{
		return m_value;
	}

	constexpr T* operator->()
	{
		return &m_value;
	}

	constexpr const T& operator*() const
	{
		return m_value;
	}

	constexpr const T* operator->() const
	{
		return &m_value;
	}

private:
	constexpr void processMembers(Operator&& op) const;

	mutable T m_value {};
};

#endif  // SERIALISABLE_H_
