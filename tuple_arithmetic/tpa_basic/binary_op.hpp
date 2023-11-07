#include "defines.hpp"
#include "basics.hpp"
#include "functions.hpp"

#pragma once

TP_ENTER_NS

// Binary operation
namespace detail {
    template<typename Op, tuple_like T1, tuple_like T2, std::size_t...I>
        requires(tpa_tuple_size_v<T1> == tpa_tuple_size_v<T2>)
    FORCE_INLINE constexpr auto apply_binary_op_impl(Op&& op, T1&& t1, T2&& t2, std::index_sequence<I...>) {
        return TP_CONVERT(std::make_tuple(op(
                    get<I>(std::forward<T1>(t1)),
                    get<I>(std::forward<T2>(t2)))...));
    }
}
template<typename Op, typename T1, typename T2>
    requires( tuple_like<T1> || tuple_like<T2> )
FORCE_INLINE constexpr auto apply_binary_op(Op&& op, T1&& v1, T2&& v2) {
    decltype(auto) tp1 = detail::broadcast<T2>(std::forward<T1>(v1));
    decltype(auto) tp2 = detail::broadcast<T1>(std::forward<T2>(v2));
    return detail::apply_binary_op_impl(
            std::forward<Op>(op),
            std::forward<decltype(tp1)>(tp1),
            std::forward<decltype(tp2)>(tp2),
            std::make_index_sequence<tpa_tuple_size_v<decltype(tp1)>>{});
}

#define TP_MAKE_BINARY_OP(FN_NAME, EXPR) \
template<typename Tp1, typename Tp2> \
    requires(tuple_like<Tp1> || tuple_like<Tp2>) \
FORCE_INLINE constexpr auto FN_NAME(Tp1&& tp1, Tp2&& tp2) { \
    return apply_binary_op( \
            [](auto&& a, auto&& b) { return (EXPR); }, \
            std::forward<Tp1>(tp1), std::forward<Tp2>(tp2)); \
}

#define TP_MAP_BINARY_FN(FN_NAME) \
    TP_MAKE_BINARY_OP(FN_NAME, FN_NAME(a, b))

#define TP_MAP_BINARY_STD_FN(FN_NAME) \
template<typename Tp1, typename Tp2> \
    requires(tuple_like<Tp1> || tuple_like<Tp2>) \
FORCE_INLINE constexpr auto FN_NAME(Tp1&& tp1, Tp2&& tp2) { \
    return apply_binary_op( \
            [](auto&& a, auto&& b) { using std::FN_NAME; return (FN_NAME(a, b)); }, \
            std::forward<Tp1>(tp1), std::forward<Tp2>(tp2)); \
}

TP_MAKE_BINARY_OP(operator+, a + b);
TP_MAKE_BINARY_OP(operator-, a - b);
TP_MAKE_BINARY_OP(operator*, a * b);
TP_MAKE_BINARY_OP(operator/, a / b);

TP_MAP_BINARY_STD_FN(min);
TP_MAP_BINARY_STD_FN(max);

// (a1, a2, ...), (b1, b2, ...) -> (a1[b1], a2[b2], ...)
template<tuple_like Tp, typename Idx>
FORCE_INLINE constexpr auto index(Tp&& tp, Idx&& idx) {
    return apply_binary_op(
            [](auto&& val, auto&& idx) { return val[idx]; },
            std::forward<Tp>(tp), std::forward<Idx>(idx));
}

// (a1, a2, ...), (b1, b2, ...) -> (a1(b1), a2(b2), ...)
template<tuple_like Tp, typename Args>
FORCE_INLINE constexpr auto invoke(Tp&& tp, Args&& args) {
    return apply_binary_op(
            [](auto&& val, auto&& arg) { return val(arg); },
            std::forward<Tp>(tp), std::forward<Args>(args));
}

// (a1, a2, ...), (b1, b2, ...) -> (a1.m(b1), a2.m(b2), ...)
template<typename T, tuple_like Tp1, tuple_like Tp2, typename Tm>
FORCE_INLINE constexpr auto m_fn2(Tp1&& tp1, Tm T::*member, Tp2&& tp2) {
    return apply_binary_op(
            [member](auto&& v1, auto&& v2) { return (v1.*member)(v2); },
            std::forward<Tp1>(tp1), std::forward<Tp2>(tp2));
}

// cross2: (a1, a2) x (b1, b2) = a1 * b2 - a2 * b1
template<tuple_like Tp1, tuple_like Tp2>
    requires(std::tuple_size_v<std::remove_cvref_t<Tp1>> == 2 && std::tuple_size_v<std::remove_cvref_t<Tp2>> == 2)
FORCE_INLINE constexpr auto cross(Tp1&& tp1, Tp2&& tp2) {
    return get<0>(tp1) * get<1>(tp2) - 
           get<1>(tp1) * get<0>(tp2);
}
// cross3: (a1, a2, a3) x (b1, b2, b3) = (a2 * b3 - a3 * b2, a3 * b1 - a1 * b3, a1 * b2 - a2 * b1)
template<tuple_like Tp1, tuple_like Tp2>
    requires(std::tuple_size_v<std::remove_cvref_t<Tp1>> == 3 && std::tuple_size_v<std::remove_cvref_t<Tp2>> == 3)
FORCE_INLINE constexpr auto cross(Tp1&& tp1, Tp2&& tp2) {
    return permute<1, 2, 0>(std::forward<Tp1>(tp1)) * permute<2, 0, 1>(std::forward<Tp2>(tp2)) -
           permute<2, 0, 1>(std::forward<Tp1>(tp1)) * permute<1, 2, 0>(std::forward<Tp2>(tp2));
}

TP_EXIT_NS
