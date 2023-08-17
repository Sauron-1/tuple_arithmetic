#include "defines.hpp"
#include <concepts>
#include <tuple>
#include <functional>
#include <type_traits>

#pragma once

TP_ENTER_NS

template<typename T, std::size_t I>
concept has_tuple_element = requires(T t) {
    typename std::tuple_element_t<I, std::remove_cvref_t<T>>;
    { std::get<I>(t) } -> std::convertible_to<std::tuple_element_t<I, std::remove_cvref_t<T>>>;
};

/**
 * Concept for tuple-like objects.
 * Requirements:
 * - std::tuple_size<T>::type is integral constant
 * - tuple element type can be accessed via std::tuple_element_t<N, T>
 * - get<N>(t) is valid for N in [0, std::tuple_size_v<T>)
 */
template<typename T>
concept tuple_like = requires(std::remove_cvref_t<T> t) {
    typename std::tuple_size<decltype(t)>::type;
    requires std::derived_from<
        std::tuple_size<decltype(t)>,
        std::integral_constant<std::size_t, std::tuple_size_v<decltype(t)>>
    >;
} && []<std::size_t...I>(std::index_sequence<I...>) {
    return (has_tuple_element<T, I> && ...);
}(std::make_index_sequence<std::tuple_size_v<std::remove_cvref_t<T>>>{});

/**
 * ref_tuple is a tuple-like object that holds lvalue references.
 */
template<typename T>
concept ref_tuple_like = tuple_like<T> &&
    []<size_t...N>(std::index_sequence<N...>) {
        return (std::is_lvalue_reference<std::tuple_element_t<N, std::remove_cvref_t<T>>>::value || ...);
    }(std::make_index_sequence<std::tuple_size_v<std::remove_cvref_t<T>>>{});

/**
 * same_type_tuple is a tuple-like object that satisfies:
 * - tuple size is 1, or
 * - all tuple elements are of the same type
 */
template<typename T>
concept same_type_tuple = tuple_like<T> &&
    (( std::tuple_size_v<std::remove_cvref_t<T>> == 1 ) ||
     []<size_t...N>(std::index_sequence<N...>) {
        return (std::is_same_v<std::tuple_element_t<N, std::remove_cvref_t<T>>,
                               std::tuple_element_t<0, std::remove_cvref_t<T>>> && ...);
     }(std::make_index_sequence<std::tuple_size_v<std::remove_cvref_t<T>>>{}));

template<typename T>
concept same_value_tuple = tuple_like<T> &&
    (( not std::is_reference_v<std::tuple_element_t<0, std::remove_cvref_t<T>>> ) &&
     ( same_type_tuple<T>)
    );

/**
 * For-loop for constexpr context.
 */
#ifndef CONSTEXPR_FOR
#define CONSTEXPR_FOR
template<auto Start, auto End, auto Inc, typename Fn, typename...Args>
    requires( Start >= End or std::invocable<Fn, std::integral_constant<decltype(Start), Start>, Args...> )
FORCE_INLINE constexpr void constexpr_for(Fn&& fn, Args&&...args) {
    if constexpr (Start < End) {
        std::invoke(std::forward<Fn>(fn), std::integral_constant<decltype(Start), Start>{}, std::forward<Args>(args)...);
        constexpr_for<Start+Inc, End, Inc>(std::forward<Fn>(fn), std::forward<Args>(args)...);
    }
}
#endif

template<typename T> struct tpa_tuple_size : public std::tuple_size<std::remove_cvref_t<T>> {};
template<typename T> static constexpr size_t tpa_tuple_size_v = tpa_tuple_size<T>::value;

TP_EXIT_NS
