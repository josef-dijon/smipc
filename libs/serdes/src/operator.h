#ifndef OPERATOR_H_
#define OPERATOR_H_

#include "calculate_actual_size.h"
#include "calculate_minimum_size.h"
#include "check_dynamicism.h"
#include "serialise.h"

#include <variant>

using Operation = std::variant<CalculateMinimumSize, CalculateActualSize, CheckDynamicism, Serialise>;

class Operator
{
public:
	explicit constexpr Operator(Operation&& operation)
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
