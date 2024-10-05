#ifndef SERIALISE_H_
#define SERIALISE_H_

#include <libsmipc/serdes/concepts.hpp>
#include <libsmipc/serdes/variable-integer.hpp>

#include <algorithm>
#include <cstddef>
#include <span>

class Serialise
{
public:
	explicit constexpr Serialise(std::span<std::byte> data) noexcept
		: m_data {data}
	{}

	template <Fundamental T>
	constexpr void operator()(T& value)
	{
		writeBytes({reinterpret_cast<const std::byte*>(&value), sizeof(T)});
	}

	template <VarInt T>
	constexpr void operator()(T& value)
	{
		writeBytes(value.bytes());
	}

	template <Enum T>
	constexpr void operator()(T& value)
	{
		const auto fundamentalValue {static_cast<std::underlying_type_t<T>>(value)};
		operator()(fundamentalValue);
	}

	template <Vector T>
	constexpr void operator()(T& value)
	{
		VariableInteger size {value.size()};
		operator()(size);
		std::for_each(std::begin(value), std::end(value), [this](auto& elem)
					  { operator()(elem); });
	}

	template <String T>
	constexpr void operator()(T& value)
	{
		VariableInteger size {value.length()};
		operator()(size);
		std::for_each(std::begin(value), std::end(value), [this](auto& elem)
					  { operator()(elem); });
	}

private:
	constexpr void writeBytes(std::span<const std::byte> data)
	{
		if (m_pointer + data.size() > m_data.size())
		{
			throw;  // Not enough space exception
		}

		std::copy_n(std::begin(data), data.size(), std::begin(m_data) + m_pointer);
		m_pointer += data.size();
	}

	std::size_t m_pointer {0};
	std::span<std::byte> m_data {};
};

#endif  // SERIALISE_H_
