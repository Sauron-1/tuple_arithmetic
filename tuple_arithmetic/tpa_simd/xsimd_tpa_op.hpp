#include <concepts>
#include <type_traits>
#include <xsimd/xsimd.hpp>
#include "../tpa_basic/basics.hpp"
#include "xsimd_cast.hpp"

#pragma once

namespace tpa {

template<tuple_like Tp, typename T, typename A>
static inline constexpr bool simd_same_size_v = sizeof(xsimd::batch<T, A>)/sizeof(T) == std::tuple_size_v<std::remove_cvref_t<Tp>>;

template<tuple_like Tp, typename T, typename A>
using final_type_simd = final_type_t<T, tuple_max_type_t<Tp>>;

template<typename T> struct max_type { using type = T; };
template<tuple_like T> struct max_type<T> { using type = tuple_max_type_t<T>; };
template<typename T, typename A> struct max_type<xsimd::batch<T, A>> { using type = T; };
template<typename T> using max_type_t = typename max_type<T>::type;

template<typename T1, typename T2>
using final_type_of = final_type_t<max_type_t<T1>, max_type_t<T2>>;

template<typename T, typename A>
FORCE_INLINE constexpr auto to_array(const xsimd::batch<T, A>& simd) {
    constexpr size_t num = sizeof(xsimd::batch<T, A>) / sizeof(T);
    alignas(sizeof(xsimd::batch<T, A>)) std::array<T, num> ret;
    simd.store_aligned(ret.data());
    return ret;
}

template<typename T, typename A>
FORCE_INLINE constexpr auto to_array_bool(const xsimd::batch_bool<T, A>& bsimd) {
    constexpr size_t N = sizeof(xsimd::batch<T, A>)/sizeof(T);
    std::array<bool, N> ret;
    uint64_t msk = bsimd.mask();
    for (size_t i = 0; i < N; ++i)
        ret[i] = msk & (1ull << i);
    return ret;
}

template<typename T>
static inline constexpr bool is_batch_or_bb = xsimd::is_batch<T>::value or xsimd::is_batch_bool<T>::value;

template<tuple_like Tp, typename T> struct is_same_sized : std::false_type {};
template<tuple_like Tp, typename T, typename A> struct is_same_sized<Tp, xsimd::batch<T, A>> {
    using type = bool;
    constexpr static bool value = simd_same_size_v<Tp, T, A>;
};
template<tuple_like Tp, typename T, typename A> struct is_same_sized<Tp, xsimd::batch_bool<T, A>> {
    using type = bool;
    constexpr static bool value = simd_same_size_v<Tp, T, A>;
};
template<tuple_like Tp, typename T>
static inline constexpr bool is_same_sized_v = is_same_sized<Tp, T>::value;

// assign
// Declare three same versions instead of use constrain to avoid ambiguous
template<tuple_like Tp, typename T, typename A>
    requires( simd_same_size_v<Tp, T, A> )
FORCE_INLINE constexpr auto assign(Tp&& tp, const xsimd::batch<T, A>& simd) {
    const T* ptr = reinterpret_cast<const T*>(&simd);
    tpa::constexpr_for<0, std::tuple_size_v<std::remove_cvref_t<Tp>>, 1>([&tp, ptr](auto I) {
        constexpr size_t i = decltype(I)::value;
        std::get<I>(std::forward<Tp>(tp)) = ptr[i];
    });
}
template<tuple_like Tp, typename T, typename A>
    requires( simd_same_size_v<Tp, T, A> )
FORCE_INLINE constexpr auto assign(Tp&& tp, xsimd::batch<T, A>&& simd) {
    const T* ptr = reinterpret_cast<const T*>(&simd);
    tpa::constexpr_for<0, std::tuple_size_v<std::remove_cvref_t<Tp>>, 1>([&tp, ptr](auto I) {
        constexpr size_t i = decltype(I)::value;
        std::get<I>(std::forward<Tp>(tp)) = ptr[i];
    });
}
template<tuple_like Tp, typename T, typename A>
    requires( simd_same_size_v<Tp, T, A> )
FORCE_INLINE constexpr auto assign(Tp&& tp, xsimd::batch<T, A>& simd) {
    const T* ptr = reinterpret_cast<const T*>(&simd);
    tpa::constexpr_for<0, std::tuple_size_v<std::remove_cvref_t<Tp>>, 1>([&tp, ptr](auto I) {
        constexpr size_t i = decltype(I)::value;
        std::get<I>(std::forward<Tp>(tp)) = ptr[i];
    });
}

template<tuple_like Tp, typename T, typename A>
    requires( simd_same_size_v<Tp, T, A> )
FORCE_INLINE constexpr auto assign(xsimd::batch<T, A>& simd, Tp&& tp) {
    simd = to_simd<T>(tp);
}

template<typename T1, typename A1, typename T2,  typename A2>
    requires( not std::is_same_v<T1, T2> )
FORCE_INLINE constexpr auto assign(xsimd::batch<T1, A1>& s1, const xsimd::batch<T2, A2>& s2) {
    constexpr size_t N = sizeof(s2) / sizeof(T2);
    alignas(sizeof(s2)) std::array<T2, N> arr;
    s2.store_aligned(arr.data());
    alignas(sizeof(s1)) auto arr1 = cast<std::remove_cvref_t<T1>>(arr);
    s1 = xsimd::batch<T1, A1>::load_aligned(arr1.data());
}


// binary op
namespace detail {
template<typename Op, typename T1, typename T2>
    requires( is_batch_or_bb<std::remove_cvref_t<T1>> or is_batch_or_bb<std::remove_cvref_t<T2>> )
FORCE_INLINE constexpr auto simd_apply_binary_op(Op&& op, T1&& v1, T2&& v2) {
    using type1 = std::remove_cvref_t<T1>;
    using type2 = std::remove_cvref_t<T2>;
    if constexpr (is_batch_or_bb<type1>) {
        using type = typename type1::value_type;
        using simd_t = std::remove_cvref_t<decltype(to_simd<type>(v2))>;
        if constexpr (is_batch_or_bb<simd_t> and std::invocable<Op, type1, simd_t>)
            return op(std::forward<T1>(v1), to_simd<type>(std::forward<T2>(v2)));
        else
            return op(to_array(std::forward<T1>(v1)), to_array(std::forward<T2>(v2)));
    }
    else {
        using type = typename type2::value_type;
        using simd_t = std::remove_cvref_t<decltype(to_simd<type>(v1))>;
        if constexpr (is_batch_or_bb<simd_t> and std::invocable<Op, simd_t, type2>)
            return op(to_simd<type>(std::forward<T1>(v1)), std::forward<T2>(v2));
        else
            return op(to_array(std::forward<T1>(v1)), to_array(std::forward<T2>(v2)));
    }
}
}

template<typename Op, typename T1, typename T2>
    requires( (tuple_like<T1> or tuple_like<T2>) &&
              ( (tuple_like<T1> and (is_same_sized_v<T1, std::remove_cvref_t<T2>>)) or
                (tuple_like<T2> and (is_same_sized_v<T2, std::remove_cvref_t<T1>>))))
FORCE_INLINE constexpr auto apply_binary_op(Op&& op, T1&& v1, T2&& v2) {
    return detail::simd_apply_binary_op(
            std::forward<Op>(op),
            std::forward<T1>(v1),
            std::forward<T2>(v2));
}

// dot product
template<typename T, typename A, tuple_like Tp>
    requires( simd_same_size_v<Tp, T, A> )
FORCE_INLINE constexpr auto dot(const xsimd::batch<T, A>& simd, Tp&& tp) {
    auto mul = simd * tp;
    if constexpr (is_batch_or_bb<decltype(mul)>)
        return xsimd::reduce_add(mul);
    else
        return sum(mul);
}

template<typename T, typename A, tuple_like Tp>
    requires( simd_same_size_v<Tp, T, A> )
FORCE_INLINE constexpr auto dot(Tp&& tp, const xsimd::batch<T, A>& simd) {
    auto mul = simd * tp;
    if constexpr (is_batch_or_bb<decltype(mul)>)
        return xsimd::reduce_add(mul);
    else
        return sum(mul);
}

template<typename T1, typename A1, typename T2, typename A2>
FORCE_INLINE constexpr auto dot(const xsimd::batch<T1, A1>& simd1, const xsimd::batch<T2, A2>& simd2) {
    return xsimd::reduce_add(simd1 * simd2);
}

// select
namespace internal {
template<typename T> struct tp_simd_size : std::integral_constant<size_t, 0> {};
template<tuple_like T> struct tp_simd_size<T> : std::tuple_size<T> {};
template<typename T, typename A> struct tp_simd_size<xsimd::batch<T, A>> : std::integral_constant<size_t, xsimd::batch<T, A>::size> {};
template<typename T, typename A> struct tp_simd_size<xsimd::batch_bool<T, A>> : std::integral_constant<size_t, xsimd::batch<T, A>::size> {};
template<typename T> constexpr inline static size_t tp_simd_size_v = tp_simd_size<T>::value;

template<typename _T1, typename _T2, typename _T3>
constexpr static inline bool can_simd_select() {
    using T1 = std::remove_cvref_t<_T1>;
    using T2 = std::remove_cvref_t<_T2>;
    using T3 = std::remove_cvref_t<_T3>;

    if constexpr (not (xsimd::is_batch_bool<T1>::value or is_batch_or_bb<T2> or is_batch_or_bb<T3>))
        return false;
    else {
        constexpr size_t s1 = tp_simd_size_v<T1>;
        constexpr size_t s2 = tp_simd_size_v<T2>;
        constexpr size_t s3 = tp_simd_size_v<T3>;
        if constexpr (s1 != 0 and s2 != 0 and s1 != s2)
            return false;
        if constexpr (s1 != 0 and s3 != 0 and s1 != s3)
            return false;
        if constexpr (s3 != 0 and s2 != 0 and s3 != s2)
            return false;
        return true;
    }
}

template<typename T1, typename T2, typename T3>
constexpr static inline bool can_simd_select_v = can_simd_select<T1, T2, T3>();

}

namespace detail {
template<typename T1, typename T2, typename T3>
    requires( internal::can_simd_select_v<T1, T2, T3> )
FORCE_INLINE constexpr auto select(T1&& v1, T2&& v2, T3&& v3) {
    using type = final_type_of<std::remove_cvref_t<T2>, std::remove_cvref_t<T3>>;
    if constexpr (xsimd::is_batch<std::remove_cvref_t<T2>>::value) {
        using simd_t = std::remove_cvref_t<decltype(to_simd<type>(v2))>;
        if constexpr (is_batch_or_bb<simd_t>)
            return xsimd::select(
                    to_simd_bool<type>(std::forward<T1>(v1)),
                    cast_simd<simd_t>(std::forward<T2>(v2)),
                    cast_simd<simd_t>(std::forward<T3>(v3)));
        else
            if constexpr (is_batch_or_bb<std::remove_cvref_t<T3>>)
                return select(
                        to_array_bool(std::forward<T1>(v1)),
                        to_array(v2),
                        to_array(v3));
            else
                return select(
                        to_array_bool(std::forward<T1>(v1)),
                        to_array(v2),
                        std::forward<T3>(v3));
    }
    else {
        using simd_t = std::remove_cvref_t<decltype(to_simd<type>(v3))>;
        if constexpr (is_batch_or_bb<simd_t>)
            return xsimd::select(
                    to_simd_bool<type>(std::forward<T1>(v1)),
                    cast_simd<simd_t>(std::forward<T2>(v2)),
                    cast_simd<simd_t>(std::forward<T3>(v3)));
        else
            return select(
                    to_array_bool(std::forward<T1>(v1)),
                    std::forward<T2>(v2),
                    to_array(v3));
    }
}
}

/**
 * Split `select` implementation into two versions to avoid ambiguous.
 * The version in tuple_arithmetic requires at least one of the
 * parameters is tuple-like.
 * Reference "Partial ordering of constraints" at https://en.cppreference.com/w/cpp/language/constraints.
 */

template<typename T1, typename T2, typename T3>
    requires( (tuple_like<T1> || tuple_like<T2> || tuple_like<T3>) and  // to override default one
              internal::can_simd_select_v<T1, T2, T3> )
FORCE_INLINE constexpr auto select(T1&& v1, T2&& v2, T3&& v3) {
    return detail::select(
            std::forward<T1>(v1),
            std::forward<T2>(v2),
            std::forward<T3>(v3));
}

template<typename T1, typename T2, typename T3>
    requires( (not (tuple_like<T1> || tuple_like<T2> || tuple_like<T3>)) and
              internal::can_simd_select_v<T1, T2, T3> )
FORCE_INLINE constexpr auto select(T1&& v1, T2&& v2, T3&& v3) {
    return detail::select(
            std::forward<T1>(v1),
            std::forward<T2>(v2),
            std::forward<T3>(v3));
}


template<typename T>
FORCE_INLINE constexpr auto to_array_deep(T&& tp) {
    if constexpr (xsimd::is_batch<std::remove_cvref_t<T>>::value)
        return to_array(tp);
    else if constexpr (tuple_like<T>)
        return apply_unary_op([](auto&& v) { return to_array_deep(v); }, std::forward<T>(tp));
    else
        return std::forward<T>(tp);
}

template<typename T, typename A>
FORCE_INLINE constexpr auto sum(const xsimd::batch<T, A>& simd) {
    return xsimd::reduce_add(simd);
}

template<typename T1, typename A1, typename T2, typename A2>
FORCE_INLINE constexpr auto operator&(const xsimd::batch_bool<T1, A1>& b1, const xsimd::batch_bool<T2, A2>& b2) {
    return xsimd::batch_bool<T1, A1>::from_mask(b1.mask() & b2.mask());
}
template<typename T1, typename A1, typename T2, typename A2>
FORCE_INLINE constexpr auto operator|(const xsimd::batch_bool<T1, A1>& b1, const xsimd::batch_bool<T2, A2>& b2) {
    return xsimd::batch_bool<T1, A1>::from_mask(b1.mask() | b2.mask());
}
template<typename T, typename A>
FORCE_INLINE constexpr auto operator~(const xsimd::batch_bool<T, A>& b) {
    constexpr size_t size = xsimd::batch_bool<T, A>::size;
    return xsimd::batch_bool<T, A>::from_mask(~b.mask() & (1ull >> (64-size)));
}


#define TPA_MAP_SIMD_BINARY_OP(NAME, EXPR) \
template<typename T1, typename A1, typename T2, typename A2> \
    requires( not std::is_same_v<T1, T2> ) \
FORCE_INLINE constexpr auto NAME(const xsimd::batch<T1, A1>& a, const xsimd::batch<T2, A2>& b) { \
    return detail::simd_apply_binary_op([](auto a, auto b) { return (EXPR); }, a, b); \
}

TPA_MAP_SIMD_BINARY_OP(operator+, a+b);
TPA_MAP_SIMD_BINARY_OP(operator-, a+b);
TPA_MAP_SIMD_BINARY_OP(operator*, a+b);
TPA_MAP_SIMD_BINARY_OP(operator/, a+b);
TPA_MAP_SIMD_BINARY_OP(dot, dot(a, b));
}

namespace xsimd {
template<typename To, typename T, typename A>
FORCE_INLINE constexpr auto cast(const xsimd::batch<T, A>& simd) {
    return tpa::to_simd<To>(simd);
}
}
