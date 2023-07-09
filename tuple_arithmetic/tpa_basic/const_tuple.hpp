#include "defines.hpp"
#include "basics.hpp"
#include <type_traits>

#pragma once

TP_ENTER_NS
// Const tuple for promotion
// A const_tuple works like a tuple with N same elements.
template<typename T, size_t N>
struct const_tuple {
    T value;

    using T_raw = std::remove_reference_t<T>;

    FORCE_INLINE operator std::array<T_raw, N>() const {
        std::array<T_raw, N> ret;
        for (auto& v : ret)
            v = value;
        return ret;
    }

    template<typename Idx>
    FORCE_INLINE T&& operator[](Idx&&) {
        return std::forward<T>(value);
    }

    template<typename Idx>
    FORCE_INLINE const T&& operator[](Idx&&) const {
        return std::forward<const T>(value);
    }
};

template<typename T> struct is_const_tuple : public std::false_type {};
template<typename T, size_t N> struct is_const_tuple<const_tuple<T, N>> : public std::true_type {};
template<typename T> static constexpr bool is_const_tuple_v = is_const_tuple<T>::value;
TP_EXIT_NS

namespace std {
template<typename T, size_t N>
struct tuple_size<TP_IN_NS(const_tuple)<T, N>> : public integral_constant<size_t, N> {};

template<size_t idx, typename T, size_t N>
struct tuple_element<idx, TP_IN_NS(const_tuple)<T, N>> {
    using type = T;
};

template<size_t idx, typename T, size_t N>
    requires (idx < N)
FORCE_INLINE constexpr T&& get(TP_IN_NS(const_tuple)<T, N>& ct) {
    return std::forward<T>(ct.value);
}

template<size_t idx, typename T, size_t N>
    requires (idx < N)
FORCE_INLINE constexpr const T&& get(const TP_IN_NS(const_tuple)<T, N>& ct) {
    return std::forward<const T>(ct.value);
}

template<size_t idx, typename T, size_t N>
    requires (idx < N)
FORCE_INLINE constexpr T&& get(TP_IN_NS(const_tuple)<T, N>&& ct) {
    return std::forward<T>(ct.value);
}
}  // namespace std
