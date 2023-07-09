#include "defines.hpp"
#include "basics.hpp"
#include "functions.hpp"

#pragma once

TP_ENTER_NS

// Assignment.
// Assignment statement must be called explicitly because
// we can't overload operator=.
namespace detail {
    // tuple-tuple assignment: same tuple size
    template<typename T1, typename T2>
    concept can_assign_tp_tp = tuple_like<T1> and tuple_like<T2>
        and std::tuple_size_v<std::remove_reference_t<T1>> == std::tuple_size_v<std::remove_reference_t<T2>>;

    // tuple-value assignment: a tuple and a scalar
    template<typename T1, typename T2>
    concept can_assign_tp_val = tuple_like<T1> and not tuple_like<T2>;

    // value-value assignment: two scalars with valid operator=
    template<typename T1, typename T2>
    concept can_assign_val_val = not tuple_like<T1> and not tuple_like<T2> and std::is_assignable_v<T1, T2>;
        //requires( T1 v1, T2 v2 ) { v1 = v2; };

    // direct assignment: any two types with valid operator=
    template<typename T1, typename T2>
    concept can_direct_assign = requires (T1 v1, T2 v2) {
        v1 = v2;
    };

    template<typename T1, typename T2>
    concept can_assign =
        can_assign_tp_tp<T1, T2> or
        can_assign_tp_val<T1, T2> or
        can_assign_val_val<T1, T2> or
        //can_direct_assign<T1, T2>;
        std::is_assignable_v<T1, T2>;
}
/**
 * Assignment operator
 */
template<typename T1, typename T2>
    requires( detail::can_assign<T1, T2> )
FORCE_INLINE constexpr auto assign(T1&& t1, T2&& t2) {
    if constexpr (detail::can_direct_assign<T1, T2>) {
        t1 = t2;
    }
    else if constexpr (detail::can_assign_tp_tp<T1, T2>) {
        constexpr_for<0, std::tuple_size_v<std::remove_reference_t<T1>>, 1>( [](auto I, auto&& v1, auto&& v2) {
            assign(get<I>(std::forward<decltype(v1)>(v1)), get<I>(std::forward<decltype(v2)>(v2)));
        }, std::forward<T1>(t1), std::forward<T2>(t2));
    }
    else if constexpr (detail::can_assign_tp_val<T1, T2>) {
        constexpr_for<0, std::tuple_size_v<std::remove_reference_t<T1>>, 1>( [](auto I, auto&& v1, auto&& v2) {
            assign(get<I>(std::forward<decltype(v1)>(v1)), std::forward<decltype(v2)>(v2));
        }, std::forward<T1>(t1), std::forward<T2>(t2));
    }
    else {
        t1 = t2;
    }
}

TP_EXIT_NS
