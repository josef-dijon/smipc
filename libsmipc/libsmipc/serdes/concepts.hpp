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

#ifndef CONCEPTS_H_
#define CONCEPTS_H_

#include <concepts>
#include <string>
#include <type_traits>
#include <vector>

template <class T>
struct is_vector: std::false_type
{};

template <class T, class A>
struct is_vector<std::vector<T, A>>: std::true_type
{};

template <class T>
struct is_string: std::false_type
{};

template <class C, class T, class A>
struct is_string<std::basic_string<C, T, A>>: std::true_type
{};

template <class T>
concept Enum = std::is_enum_v<T>;

template <class T>
concept Fundamental = std::is_integral_v<T> || std::is_floating_point_v<T>;

template <class T>
concept Vector = is_vector<T>::value;

template <class T>
concept String = is_string<T>::value;

#endif  // CONCEPTS_H_
