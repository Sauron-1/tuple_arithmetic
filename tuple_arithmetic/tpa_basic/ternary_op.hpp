#include "defines.hpp"
#include "basics.hpp"
#include "functions.hpp"

#pragma once

TP_ENTER_NS

// Ternary operation
namespace detail {
    template<typename Op, tuple_like T1, tuple_like T2, tuple_like T3, std::size_t...I>
        requires(tpa_tuple_size_v<T1> == tpa_tuple_size_v<T2> and tpa_tuple_size_v<T1> == tpa_tuple_size_v<T3>)
    FORCE_INLINE constexpr auto apply_ternary_op_impl(Op&& op, T1&& t1, T2&& t2, T3&& t3, std::index_sequence<I...>) {
        using std::get;
        return TP_CONVERT(std::make_tuple(op(get<I>(t1), get<I>(t2), get<I>(t3))...));
    }
}
template<typename Op, typename T1, typename T2, typename T3>
    requires( tuple_like<T1> || tuple_like<T2> || tuple_like<T3> )
FORCE_INLINE constexpr auto apply_ternary_op(Op&& op, T1&& v1, T2&& v2, T3&& v3) {
    decltype(auto) tp1 = detail::broadcast<T2, T3>(std::forward<T1>(v1));
    decltype(auto) tp2 = detail::broadcast<T1, T3>(std::forward<T2>(v2));
    decltype(auto) tp3 = detail::broadcast<T1, T2>(std::forward<T3>(v3));
    return detail::apply_ternary_op_impl(
            std::forward<Op>(op),
            std::forward<decltype(tp1)>(tp1),
            std::forward<decltype(tp2)>(tp2),
            std::forward<decltype(tp3)>(tp3),
            std::make_index_sequence<std::tuple_size_v<std::remove_cvref_t<decltype(tp1)>>>{});
}

#define TP_MAKE_TERNARY_OP(FN_NAME, EXPR) \
template<typename Tp1, typename Tp2, typename Tp3> \
    requires(tuple_like<Tp1> || tuple_like<Tp2> || tuple_like<Tp3>) \
FORCE_INLINE constexpr auto FN_NAME(Tp1&& tp1, Tp2&& tp2, Tp3&& tp3) { \
    return apply_ternary_op( \
            [](auto&& a, auto&& b, auto&& c) { return (EXPR); }, \
            std::forward<Tp1>(tp1), \
            std::forward<Tp2>(tp2), \
            std::forward<Tp3>(tp3)); \
}

#define TP_MAP_TERNARY_FN(FN_NAME) \
    TP_MAKE_TERNARY_OP(FN_NAME, FN_NAME(a, b, c))
#define TP_MAP_TERNARY_STD_FN(FN_NAME) \
    TP_MAKE_TERNARY_OP(FN_NAME, std::FN_NAME(a, b, c))

template<typename T1, typename T2>
decltype(auto) select(bool cond, T1&& if_true, T2&& if_false) {
    return cond ? if_true : if_false;
}

TP_MAKE_TERNARY_OP(select, select(a, b, c));

TP_EXIT_NS
