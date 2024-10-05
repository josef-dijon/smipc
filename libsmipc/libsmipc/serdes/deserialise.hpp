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

#ifndef DESERIALISE_H_
#define DESERIALISE_H_

#include <libsmipc/serdes/concepts.hpp>
#include <libsmipc/serdes/variable-integer.hpp>

#include <algorithm>
#include <array>
#include <cstddef>
#include <iterator>
#include <span>

class Deserialise
{
public:
	explicit constexpr Deserialise(std::span<const std::byte> data) noexcept
		: m_data {data}
	{}

	template <Fundamental T>
	constexpr void operator()(T& value)
	{
		readBytes({reinterpret_cast<std::byte*>(&value), sizeof(T)});
	}

	template <VarInt T>
	constexpr void operator()(T& value)
	{
		value = VariableInteger(m_data.data() + m_pointer);
		m_pointer += value.size();
	}

	template <Enum T>
	constexpr void operator()(T& value)
	{
		std::underlying_type_t<T> fundamentalValue {};
		operator()(fundamentalValue);
		value = static_cast<T>(fundamentalValue);
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
	constexpr void readBytes(std::span<std::byte> data)
	{
		std::copy_n(std::begin(m_data) + m_pointer, data.size(), std::begin(data));
		m_pointer += data.size();
	}

	std::span<const std::byte> m_data;
	std::size_t m_pointer {0u};
};

#endif  // DESERIALISE_H_
