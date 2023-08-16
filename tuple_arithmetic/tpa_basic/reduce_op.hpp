#include "defines.hpp"
#include "basics.hpp"
#include "functions.hpp"
#include "binary_op.hpp"

#pragma once

TP_ENTER_NS

// Reduce operation
namespace detail {
    template<typename Op>
    struct ReduceFunction {
        Op m_op;
        ReduceFunction(Op&& op) : m_op(std::move(op)) {}
        template<typename T>
        FORCE_INLINE decltype(auto) operator()(T&& v) {
            return std::forward<T>(v);
        }
        template<typename T1, typename...T>
        FORCE_INLINE auto operator()(T1&& v1, T&&...v) {
            return m_op(std::forward<T1>(v1), operator()(std::forward<T>(v)...));
        }
    };
}

// (a1, a2, ...) -> op(a1, op(a2, ...))
template<typename Op, tuple_like Tp>
    requires( std::tuple_size_v<std::remove_reference_t<Tp>> > 0 )
FORCE_INLINE constexpr auto reduce(Op&& op, Tp&& tp) {
    detail::ReduceFunction<Op> reduce_op(std::forward<Op>(op));
    return std::apply(reduce_op, tp);
}

// (a1, a2, ...) -> a1 + a2 + ...
template<tuple_like Tp>
FORCE_INLINE constexpr auto sum(Tp&& tp) {
    return reduce(
            [](auto&& a, auto&& b) { return a + b; },
            std::forward<Tp>(tp));
}

// (a1, a2, ...) -> a1 * a2 * ...
template<tuple_like Tp>
FORCE_INLINE constexpr auto prod(Tp&& tp) {
    return reduce(
            [](auto&& a, auto&& b) { return a * b; },
            std::forward<Tp>(tp));
}

// (a1, a2, ...) -> a1 || a2, || ...
template<tuple_like Tp>
FORCE_INLINE constexpr auto any(Tp&& tp) {
    return reduce(
            [](auto&& a, auto&& b) { return a || b; },
            std::forward<Tp>(tp));
}

// (a1, a2, ...) -> a1 && a2, && ...
template<tuple_like Tp>
FORCE_INLINE constexpr auto all(Tp&& tp) {
    return reduce(
            [](auto&& a, auto&& b) { return a && b; },
            std::forward<Tp>(tp));
}

// (a1, a2, ...) -> min(a1, min(a2, ...))
template<tuple_like Tp>
FORCE_INLINE constexpr auto reduce_min(Tp&& tp) {
    using std::min;
    return reduce(
            [](auto&& a, auto&& b) { return b < a ? b : a; },
            std::forward<Tp>(tp));
}

// (a1, a2, ...) -> max(a1, max(a2, ...))
template<tuple_like Tp>
FORCE_INLINE constexpr auto reduce_max(Tp&& tp) {
    using std::max;
    return reduce(
            [](auto&& a, auto&& b) { return b > a ? b : a; },
            std::forward<Tp>(tp));
}


// dot product
template<tuple_like Tp1, tuple_like Tp2>
    requires(std::tuple_size_v<std::remove_cvref_t<Tp1>> == std::tuple_size_v<std::remove_cvref_t<Tp2>>)
FORCE_INLINE constexpr auto dot(Tp1&& tp1, Tp2&& tp2) {
    return sum(tp1 * tp2);
}

TP_EXIT_NS
