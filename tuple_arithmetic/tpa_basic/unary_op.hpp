#include "defines.hpp"
#include "basics.hpp"

#pragma once

TP_ENTER_NS

// Unary operation
namespace detail {
    template<typename Op, tuple_like Tp, std::size_t...I>
    FORCE_INLINE constexpr auto apply_unary_op(Op&& op, Tp&& tp, std::index_sequence<I...>) {
        return TP_CONVERT(std::make_tuple(op(std::get<I>(tp))...));
    }
}
template<typename Op, tuple_like Tp>
FORCE_INLINE constexpr auto apply_unary_op(Op&& op, Tp&& tp) {
    return detail::apply_unary_op(
            std::forward<Op>(op), std::forward<Tp>(tp),
            std::make_index_sequence<std::tuple_size<std::remove_reference_t<Tp>>::value>{});
}

// Foreach: same as unary op, but use forward_as_tuple instead of make_tuple.
namespace detail {
    template<typename Op, typename Tp, std::size_t...I>
    FORCE_INLINE constexpr auto foreach(Op&& op, Tp&& tp, std::index_sequence<I...>) {
        return TP_CONVERT(std::forward_as_tuple(op(std::get<I>(tp))...));
    }
}
template<typename Op, tuple_like Tp>
FORCE_INLINE constexpr auto foreach(Tp&& tp, Op&& op) {
    return detail::foreach(
            std::forward<Op>(op), std::forward<Tp>(tp),
            std::make_index_sequence<std::tuple_size<std::remove_reference_t<Tp>>::value>{});
}

// functions
// (a1, a2, ...) -> (T(a1), T(a2), ...)
template<typename T, tuple_like Tp>
FORCE_INLINE constexpr auto cast(Tp&& tp) {
    return apply_unary_op(
            [](auto&& v) { return T(v); },
            std::forward<Tp>(tp));
}

// (a1, a2, ...) -> (a1.m, a2.m, ...)
template<typename T, tuple_like Tp, typename Tm>
FORCE_INLINE constexpr auto m_val(Tp&& tp, Tm T::*member) {
    return apply_unary_op(
            [member](auto&& v) { return v.*member; },
            std::forward<Tp>(tp));
}

#define TP_MAKE_UNARY_OP(FN_NAME, EXPR) \
template<tuple_like Tp> \
FORCE_INLINE constexpr auto FN_NAME(Tp&& tp) { \
    return apply_unary_op( \
            [](auto&& a) { return (EXPR); }, \
            std::forward<Tp>(tp)); \
}

#define TP_MAP_UNARY_FN(FN_NAME) \
    TP_MAKE_UNARY_OP(FN_NAME, FN_NAME(a))

#define TP_MAP_UNARY_STD_FN(FN_NAME) \
    TP_MAKE_UNARY_OP(FN_NAME, std::FN_NAME(a))

#define TP_MAP_METHOD(FN_NAME) \
template<tuple_like Tp, typename...T> \
FORCE_INLINE constexpr auto FN_NAME(Tp&& tp, T&&...args) { \
    return apply_unary_op( \
            [args...](auto&& v) { return v.FN_NAME(args...); }, \
            std::forward<Tp>(tp)); \
}

#define TP_MAP_FUNCTION(FN_NAME) \
template<tuple_like Tp, typename...T> \
FORCE_INLINE constexpr auto FN_NAME(Tp&& tp, T&&...args) { \
    return apply_unary_op( \
            [args...](auto&& v) { return FN_NAME(v, args...); }, \
            std::forward<Tp>(tp)); \
}

TP_MAKE_UNARY_OP(operator-, -a);

// (a1, a2, ...) -> (a1.m(args...), a2.m(args...), ...)
template<typename T, tuple_like Tp, typename Tm, typename...Targs>
FORCE_INLINE constexpr auto m_fn1(Tp&& tp, Tm T::*member, Targs&&...args) {
    return apply_unary_op(
            [member, args...](auto&& v) { return (v.*member)(args...); },
            std::forward<Tp>(tp));
}

// First N
namespace detail {
template<typename Tp, size_t...I>
FORCE_INLINE constexpr auto firstN_impl(Tp&& tp, std::index_sequence<I...>) {
    return permute<I...>(std::forward<Tp>(tp));
}
}
template<size_t N, tuple_like Tp>
FORCE_INLINE constexpr auto firstN(Tp&& tp) {
    return detail::firstN_impl(std::forward<Tp>(tp), std::make_index_sequence<N>{});
}

TP_EXIT_NS
