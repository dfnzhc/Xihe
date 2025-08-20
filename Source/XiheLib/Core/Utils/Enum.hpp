/**
 * @File Enum.hpp
 * @Author dfnzhc (https://github.com/dfnzhc)
 * @Date 2025/8/3
 * @Brief This file is part of Xihe.
 */

#pragma once

#include <type_traits>

namespace xihe {
// clang-format off
#define XIHE_ENUM_CLASS_OPERATORS(enumType)                                                                                              \
inline constexpr enumType operator& (enumType a, enumType b) { return static_cast<enumType>(static_cast<int>(a)& static_cast<int>(b)); } \
inline constexpr enumType operator| (enumType a, enumType b) { return static_cast<enumType>(static_cast<int>(a)| static_cast<int>(b)); } \
inline constexpr enumType& operator|= (enumType& a, enumType b) { a = a | b; return a; };                                                \
inline constexpr enumType& operator&= (enumType& a, enumType b) { a = a & b; return a; };                                                \
inline constexpr enumType  operator~ (enumType a) { return static_cast<enumType>(~static_cast<int>(a)); }                                \
inline constexpr bool IsSet(enumType val, enumType flag) { return (val & flag) != static_cast<enumType>(0); }                            \
inline constexpr void FlipEnumBit(enumType& val, enumType flag) { val = IsSet(val, flag) ? (val & (~flag)) : (val | flag); }
// clang-format on

template <typename EnumType>
using enum_ut = std::underlying_type_t<EnumType>;

template <typename EnumType>
constexpr enum_ut<EnumType> EnumValue(EnumType a)
{
    return static_cast<enum_ut<EnumType>>(a);
}
} // namespace xihe
