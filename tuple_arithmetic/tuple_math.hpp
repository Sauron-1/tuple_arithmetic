/**
 * This header provide more math functions by overloading
 * functions in "cmath".
 *
 * CAUTION: Using this header might break default behaver
 * of tuple comparison in C++23. You can define TP_NO_COMPARE
 * to avoid this.
 */
#include <cmath>
#include "tuple_arithmetic.hpp"

#pragma once

#if defined(TP_NAMESPACE)
namespace TP_NAMESPACE {
#endif

// unary
TP_MAP_UNARY_STD_FN(fpclassify);
TP_MAP_UNARY_STD_FN(isfinite);
TP_MAP_UNARY_STD_FN(isnan);
TP_MAP_UNARY_STD_FN(isnormal);

TP_MAP_UNARY_STD_FN(abs);
TP_MAP_UNARY_STD_FN(fabs);

TP_MAP_UNARY_STD_FN(exp);
TP_MAP_UNARY_STD_FN(exp2);
TP_MAP_UNARY_STD_FN(expm1);
TP_MAP_UNARY_STD_FN(log);
TP_MAP_UNARY_STD_FN(log10);
TP_MAP_UNARY_STD_FN(log2);
TP_MAP_UNARY_STD_FN(log1p);

TP_MAP_UNARY_STD_FN(sqrt);
TP_MAP_UNARY_STD_FN(cbrt);

TP_MAP_UNARY_STD_FN(sin);
TP_MAP_UNARY_STD_FN(cos);
TP_MAP_UNARY_STD_FN(tan);
TP_MAP_UNARY_STD_FN(asin);
TP_MAP_UNARY_STD_FN(acos);
TP_MAP_UNARY_STD_FN(atan);
TP_MAP_UNARY_STD_FN(atan2);

TP_MAP_UNARY_STD_FN(sinh);
TP_MAP_UNARY_STD_FN(cosh);
TP_MAP_UNARY_STD_FN(tanh);
TP_MAP_UNARY_STD_FN(asinh);
TP_MAP_UNARY_STD_FN(acosh);
TP_MAP_UNARY_STD_FN(atanh);

TP_MAP_UNARY_STD_FN(erf);
TP_MAP_UNARY_STD_FN(erfc);
TP_MAP_UNARY_STD_FN(tgamma);
TP_MAP_UNARY_STD_FN(lgamma);

TP_MAP_UNARY_STD_FN(ceil);
TP_MAP_UNARY_STD_FN(floor);
TP_MAP_UNARY_STD_FN(trunc);
TP_MAP_UNARY_STD_FN(round);
TP_MAP_UNARY_STD_FN(nearbyint);
TP_MAP_UNARY_STD_FN(rint);
TP_MAP_UNARY_STD_FN(lrint);
TP_MAP_UNARY_STD_FN(llrint);

TP_MAKE_UNARY_OP(operator~, ~a);
TP_MAKE_UNARY_OP(operator!, !a);

// binary
TP_MAKE_BINARY_OP(operator|, a | b);
TP_MAKE_BINARY_OP(operator&, a & b);
TP_MAKE_BINARY_OP(operator<, a < b);
TP_MAKE_BINARY_OP(operator<=, a <= b);
TP_MAKE_BINARY_OP(operator>, a > b);
TP_MAKE_BINARY_OP(operator>=, a >= b);
TP_MAKE_BINARY_OP(operator==, a == b);
TP_MAKE_BINARY_OP(operator<=>, a <=> b);
TP_MAKE_BINARY_OP(operator||, a || b);
TP_MAKE_BINARY_OP(operator&&, a && b);

TP_MAP_BINARY_STD_FN(pow);

TP_MAP_BINARY_STD_FN(fdim);
TP_MAP_BINARY_STD_FN(fmin);
TP_MAP_BINARY_STD_FN(fmax);

TP_MAP_BINARY_STD_FN(hypot);
TP_MAP_BINARY_STD_FN(fmod);
TP_MAP_BINARY_STD_FN(remainder);

// ternary
TP_MAP_TERNARY_STD_FN(hypot);
TP_MAP_TERNARY_STD_FN(fma);
TP_MAP_TERNARY_STD_FN(lerp);

// other operators
template<tuple_like Tp>
FORCE_INLINE constexpr auto norm(Tp&& tp) {
    return std::sqrt(dot(std::forward<Tp>(tp), std::forward<Tp>(tp)));
}

template<size_t L, tuple_like Tp>
FORCE_INLINE constexpr auto norm(Tp&& tp) {
    if constexpr (L == 0) {
        return std::tuple_size_v<std::remove_cvref_t<Tp>>;
    }
    else if constexpr (L == 1) {
        return sum(abs(std::forward<Tp>(tp)));
    }
    else if constexpr (L == 2) {
        return norm(tp);
    }
    else if constexpr (L == 3) {
        return std::cbrt(sum(powi<3>(abs(std::forward<Tp>(tp)))));
    }
    else {
        if constexpr (L % 2 == 0) {
            return std::pow(sum(powi<L>(std::forward<Tp>(tp))), 1.0/L);
        }
        else {
            return std::pow(sum(powi<L>(abs(std::forward<Tp>(tp)))), 1.0/L);
        }
    }
}

template<tuple_like Tp>
FORCE_INLINE constexpr auto normalize(Tp&& tp) {
    return tp / norm(std::forward<Tp>(tp));
}

#if defined(TP_NAMESPACE)
}  // namespace TP_NAMESPACE

#define TP_USE_OPS \
using TP_NAMESPACE::operator~; \
using TP_NAMESPACE::operator!; \
using TP_NAMESPACE::operator|; \
using TP_NAMESPACE::operator&; \
using TP_NAMESPACE::operator<; \
using TP_NAMESPACE::operator>; \
using TP_NAMESPACE::operator==; \
using TP_NAMESPACE::operator<=; \
using TP_NAMESPACE::operator>=; \
using TP_NAMESPACE::operator<=>; \
using TP_NAMESPACE::operator||; \
using TP_NAMESPACE::operator&&;

#if defined(TP_DEFAULT_USE_OPS)
TP_USE_OPS
#endif

#endif  // defined(TP_NAMESPACE)
