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

#ifndef OPERATOR_H_
#define OPERATOR_H_

#include <libsmipc/serdes/calculate-actual-size.hpp>
#include <libsmipc/serdes/calculate-minimum-size.hpp>
#include <libsmipc/serdes/check-dynamicism.hpp>
#include <libsmipc/serdes/deserialise.hpp>
#include <libsmipc/serdes/serialise.hpp>

#include <variant>

using Operation = std::variant<
	CalculateMinimumSize,
	CalculateActualSize,
	CheckDynamicism,
	Serialise,
	Deserialise>;

class Operator
{
public:
	explicit constexpr Operator(Operation&& operation) noexcept
		: m_operation {operation}
	{}

	template <class T>
	constexpr void operator()(T& value)
	{
		std::visit([this, &value](auto&& op)
				   {
				using Op = std::decay_t<decltype(op)>;
				std::get<Op>(m_operation)(value); },
				   m_operation);
	}

private:
	Operation m_operation;
};

#endif  // OPERATOR_H_
