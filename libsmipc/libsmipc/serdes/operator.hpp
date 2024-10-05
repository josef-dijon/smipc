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
