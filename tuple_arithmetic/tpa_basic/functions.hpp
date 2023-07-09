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
        return std::array<std::tuple_element_t<0, T>, sizeof...(N)>{ get<N>(tp)... };
    }
}

template<tuple_like T>
FORCE_INLINE decltype(auto) to_array(T&& tp) {
    if constexpr (same_value_tuple<T> and not is_const_tuple_v<std::remove_cvref_t<T>>) {
        return detail::to_array_impl(std::forward<T>(tp), std::make_index_sequence<std::tuple_size_v<T>>{});
    }
    else {
        return std::forward<T>(tp);
    }
}

// Permutation
template<size_t...Idx, tuple_like Tp>
FORCE_INLINE constexpr auto permute(Tp&& tp) {
    return TP_CONVERT(std::forward_as_tuple(std::get<Idx>(std::forward<Tp>(tp))...));
}


// constexpr integer pow
namespace detail {
    template<typename T>
    concept has_one_elem = requires {
        std::remove_reference_t<T>(1);
    };
}
template<size_t N, typename T>
    requires( N > 0 || detail::has_one_elem<T> )
FORCE_INLINE constexpr decltype(auto) powi(T&& val) {
    if constexpr (N == 0)
        return std::remove_reference_t<T>(1);
    else if constexpr (N == 1)
        return std::forward<T>(val);
    else {
        auto&& half = powi<N/2>(std::forward(val));
        if constexpr (N % 2 == 0)
            return half * half;
        else
            return half * half * val;
    }
}

TP_EXIT_NS
