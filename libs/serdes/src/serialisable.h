#ifndef SERIALISABLE_H_
#define SERIALISABLE_H_

#include "operator.h"

#include <span>

/**
 * A wrapper class to make c++ class/structs serialisable.
 *
 * This class template is used to wrap a user struct to make it serialisable.
 * The user must implement the processMembers function for their specialisation
 * of this class.
 */
template <class T>
class Serialisable
{
public:
	/**
	 * Serialise the struct into a vector of bytes.
	 *
	 * @return serialised vector of bytes
	 */
	constexpr auto serialise() const -> std::vector<std::byte>
	{
		std::vector<std::byte> data {};
		data.resize(getActualSize());
		processMembers(Operator(Serialise(data)));
		return data;
	}

	/**
	 * Serialise into a provided span.
	 *
	 * @param data A std::span to which the data will be serialised.
	 */
	constexpr void serialiseInto(std::span<std::byte> data) const
	{
		processMembers(Operator(Serialise(data)));
	}

	constexpr void deserialise(std::span<const std::byte> data)
	{
		processMembers(Operator(Deserialise(data)));
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
