#include "defines.hpp"
#include <type_traits>
#include <array>
#include <tuple>

#pragma once

TP_ENTER_NS
using std::size_t;
// Const tuple for promotion
// A const_tuple works like a tuple with N same elements.
template<typename T, size_t N>
struct const_tuple {
    T value;

    using T_raw = std::remove_reference_t<T>;
    using value_type = T;

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
    requires(idx < N)
struct tuple_element<idx, TP_IN_NS(const_tuple)<T, N>> {
    using type = T&&;
};

template<size_t idx, typename T>
    requires( idx < tuple_size_v<remove_cvref_t<T>> and TP_IN_NS(is_const_tuple_v)<remove_cvref_t<T>> )
FORCE_INLINE constexpr decltype(auto) get(T&& ct) {
    return ct[idx];
}
}  // namespace std
