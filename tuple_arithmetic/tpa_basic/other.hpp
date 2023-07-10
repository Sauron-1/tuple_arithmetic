#include "defines.hpp"
#include "basics.hpp"
#include "const_tuple.hpp"
#include "unary_op.hpp"
#include "binary_op.hpp"
#include <type_traits>

#pragma once

TP_ENTER_NS

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
        auto&& half = powi<N/2>(std::forward<T>(val));
        if constexpr (N % 2 == 0)
            return half * half;
        else
            return half * half * val;
    }
}

TP_EXIT_NS
