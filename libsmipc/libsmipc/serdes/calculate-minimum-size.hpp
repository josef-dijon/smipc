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

#ifndef CALCULATE_MINIMUM_SIZE_H_
#define CALCULATE_MINIMUM_SIZE_H_

#include <libsmipc/serdes/concepts.hpp>
#include <libsmipc/serdes/variable-integer.hpp>

class CalculateMinimumSize
{
public:
	explicit constexpr CalculateMinimumSize(std::size_t& size)
		: m_size {size}
	{}

	template <Fundamental T>
	constexpr void operator()(T& value)
	{
		m_size += sizeof(T);
	}

	template <VarInt T>
	constexpr void operator()(T& value)
	{
		m_size += value.size();
	}

	template <Enum T>
	constexpr void operator()(T& value)
	{
		m_size += sizeof(std::underlying_type_t<T>);
	}

	template <Vector T>
	constexpr void operator()(T& value)
	{
		VariableInteger size {std::size_t {0u}};
		operator()(size);
	}

	template <String T>
	constexpr void operator()(T& value)
	{
		VariableInteger size {std::size_t {0u}};
		operator()(size);
	}

private:
	std::size_t& m_size;
};

#endif  // CALCULATE_MINIMUM_SIZE_H_
