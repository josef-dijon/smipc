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

#ifndef SERIALISABLE_H_
#define SERIALISABLE_H_

#include <libsmipc/serdes/operator.hpp>

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
		data.resize(getSize());
		processMembers(Operator {Serialise {data}});
		return data;
	}

	/**
	 * Serialise into a provided span.
	 *
	 * @param data A std::span to which the data will be serialised.
	 */
	constexpr void serialiseInto(std::span<std::byte> data) const
	{
		processMembers(Operator {Serialise {data}});
	}

	/**
	 * Deserialise the struct from a vector of bytes.
	 *
	 * @param data A span of bytes to deserialise from.
	 */
	constexpr void deserialise(std::span<const std::byte> data)
	{
		processMembers(Operator {Deserialise {data}});
	}

	/**
	 * Get the minimum size of the serialised data.
	 *
	 * This function calculates the minimum size of the serialised data. It
	 * assumes all dynamically sized data is empty. This is useful for pre-
	 * allocating memory for serialisation, when getting the actual size is
	 * not possible or too expensive.
	 *
	 * @return The minimum size of the serialised data.
	 */
	constexpr auto getMinimumSize() const -> std::size_t
	{
		std::size_t size {};
		processMembers(Operator {CalculateMinimumSize {size}});
		return size;
	}

	/**
	 * Get the actual size of the serialised data.
	 *
	 * This function calculates the actual size of the serialised data. It
	 * takes into account the actual size of all the data members. This is
	 * useful for pre-allocating memory for serialisation, when getting the
	 * minimum size is not possible or too expensive.
	 *
	 * @return The actual size of the serialised data.
	 */
	constexpr auto getSize() const -> std::size_t
	{
		std::size_t size {};
		processMembers(Operator {CalculateActualSize {size}});
		return size;
	}

	/**
	 * Check if the struct has any dynamic data members.
	 *
	 * This function checks if the struct has any dynamic data members. It
	 * returns true if there are any dynamic data members, otherwise false.
	 *
	 * @return True if the struct has dynamic data members, otherwise false.
	 */
	constexpr auto isDynamic() const -> bool
	{
		bool dynamic {false};
		processMembers(Operator {CheckDynamicism {dynamic}});
		return dynamic;
	}

	/**
	 * Check if the struct is entirely composed of static data members.
	 *
	 * This function checks if the struct is entirely composed of static data
	 * members. It returns true if all the data members are static, otherwise
	 * false.
	 *
	 * @return True if the struct has static data members, otherwise false.
	 */
	constexpr auto isStatic() const -> bool
	{
		return ! isDynamic();
	}

	/**
	 * Dereference accessor for the wrapped struct.
	 *
	 * This function allows the user to access the wrapped struct directly.
	 *
	 * @return A reference to the wrapped struct.
	 */
	constexpr T& operator*()
	{
		return m_struct;
	}

	/**
	 * Dereference accessor for the wrapped struct.
	 *
	 * This function allows the user to access the wrapped struct directly.
	 *
	 * @return A pointer to the wrapped struct.
	 */
	constexpr T* operator->()
	{
		return &m_struct;
	}

	/**
	 * Dereference accessor for the wrapped struct.
	 *
	 * This function allows the user to access the wrapped struct directly.
	 *
	 * @return A reference to the wrapped struct.
	 */
	constexpr const T& operator*() const
	{
		return m_struct;
	}

	/**
	 * Dereference accessor for the wrapped struct.
	 *
	 * This function allows the user to access the wrapped struct directly.
	 *
	 * @return A pointer to the wrapped struct.
	 */
	constexpr const T* operator->() const
	{
		return &m_struct;
	}

private:
	/**
	 * Process the members of the struct.
	 *
	 * This function is used to process the members of the struct. It is
	 * implemented by the user for their specialisation of this class.
	 *
	 * @param op The operator to use to process the members.
	 */
	constexpr void processMembers(Operator&& op) const;

	/**
	 * The wrapped struct. Mutable to allow for const member function where
	 * appropriate.
	 */
	mutable T m_struct {};
};

#endif  // SERIALISABLE_H_
