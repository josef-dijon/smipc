#ifndef CONCEPTS_H_
#define CONCEPTS_H_

#include <concepts>
#include <cstddef>
#include <cstdint>
#include <string>
#include <type_traits>
#include <variant>
#include <vector>

template <class T>
struct is_vector: std::false_type
{};

template <class T, class A>
struct is_vector<std::vector<T, A>>: std::true_type
{};

template <class T>
constexpr bool is_vector_v = is_vector<T>::value;

template <class T>
struct is_string: std::false_type
{};

template <class C, class T, class A>
struct is_string<std::basic_string<C, T, A>>: std::true_type
{};

template <class T>
constexpr bool is_string_v = is_string<T>::value;

template <class T>
concept Enum = std::is_enum_v<T>;

template <class T>
concept Fundamental = std::is_integral_v<T> || std::is_floating_point_v<T>;

template <class T>
concept Vector = is_vector_v<T>;

template <class T>
concept String = is_string_v<T>;

#endif  // CONCEPTS_H_
