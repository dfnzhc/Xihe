/**
 * @File Polynomial.hpp
 * @Author dfnzhc (https://github.com/dfnzhc)
 * @Date 2025/8/4
 * @Brief This file is part of Xihe.
 */

#pragma once

#include "Core/Math/Common/Basic.hpp"

namespace xihe {
template <typename T, typename C>
XIHE_ALWAYS_INLINE T EvaluatePolynomial(T, C c)
{
    return As<T>(c);
}

template <typename T, typename C, typename... Args>
XIHE_ALWAYS_INLINE T EvaluatePolynomial(T t, C c, Args... cRemaining)
{
    return FMA(t, As<T>(EvaluatePolynomial(t, cRemaining...)), As<T>(c));
}

/**
 * @brief (a * b) - (c * d)
 */
template <cArithmeticType Ta, cArithmeticType Tb, cArithmeticType Tc, cArithmeticType Td>
XIHE_ALWAYS_INLINE auto DifferenceOfProducts(Ta a, Tb b, Tc c, Td d)
{
    using T    = std::common_type_t<Ta, Tb, Tc, Td>;
    auto cd    = As<T>(c * d);
    auto dop   = FMA(As<T>(a), As<T>(b), -cd);
    auto error = FMA(-As<T>(c), As<T>(d), cd);
    return dop + error;
}

/**
 * @brief (a * b) + (c * d)
 */
template <cArithmeticType Ta, cArithmeticType Tb, cArithmeticType Tc, cArithmeticType Td>
XIHE_ALWAYS_INLINE auto SumOfProducts(Ta a, Tb b, Tc c, Td d)
{
    using T    = std::common_type_t<Ta, Tb, Tc, Td>;
    auto cd    = As<T>(c * d);
    auto sop   = FMA(As<T>(a), As<T>(b), cd);
    auto error = FMA(As<T>(c), As<T>(d), -cd);
    return sop + error;
}
} // namespace xihe
