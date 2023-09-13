#include "defines.hpp"
#include "basics.hpp"
#include "const_tuple.hpp"
#include <type_traits>

#pragma once

TP_ENTER_NS

namespace detail {
    /**
     * Get the tuple size of the first tuple-like object in Ts.
     */
    template<typename T, typename...Ts>
    constexpr size_t tuple_size_of() {
        using T_raw = std::remove_cvref_t<T>;
        if constexpr (tuple_like<T_raw>) {
            return std::tuple_size_v<T_raw>;
        }
        else if constexpr (sizeof...(Ts) > 0) {
            return tuple_size_of<Ts...>();
        }
        else {
            return 0;
        }
    }

    /**
     * Promote scalar to tuple (const_tuple<T, N>), N
     * is decided by Trefs.
     */
    template<typename...Tref, typename T>
        requires(tuple_like<T> || (... || tuple_like<Tref>))
    FORCE_INLINE constexpr decltype(auto) broadcast(T&& t) {
        using T_raw = std::remove_cvref_t<T>;
        if constexpr (tuple_like<T_raw>) {
            return std::forward<T>(t);
        }
        else {
            return const_tuple<T&&, tuple_size_of<Tref...>()>{std::forward<T>(t)};
        }
    }
}

/**
 * Convert tuple with same non-refernece value types to an std::array.
 */
namespace detail {
    template<tuple_like T, size_t...N>
    FORCE_INLINE auto to_array_impl(T&& tp, std::index_sequence<N...>) {
        return std::array<std::tuple_element_t<0, std::remove_cvref_t<T>>, sizeof...(N)>{ get<N>(tp)... };
    }
}

template<tuple_like T>
FORCE_INLINE decltype(auto) to_array(T&& tp) {
    using T_nocv = std::remove_cvref_t<T>;
    if constexpr (same_value_tuple<T_nocv> and not is_const_tuple_v<T_nocv>) {
        return detail::to_array_impl(std::forward<T>(tp), std::make_index_sequence<std::tuple_size_v<T_nocv>>{});
    }
    else {
        return std::forward<T>(tp);
    }
}

template<typename T, tuple_like Tp>
FORCE_INLINE auto repeat_as(T&& t, Tp&&) {
    return const_tuple<T&&, std::tuple_size_v<std::remove_cvref_t<Tp>>>{std::forward<T>(t)};
}

template<typename T, typename To>
    requires( not tuple_like<To> and std::is_convertible_v<std::remove_cvref_t<T>, std::remove_cvref_t<To>> )
FORCE_INLINE auto repeat_as(T&& t, To&&) {
    return std::remove_cvref_t<To>(std::forward<T>(t));
}

// Permutation
template<size_t...Idx, tuple_like Tp>
FORCE_INLINE constexpr auto permute(Tp&& tp) {
    using std::get;
    return TP_CONVERT(std::forward_as_tuple(get<Idx>(std::forward<Tp>(tp))...));
}

TP_EXIT_NS
