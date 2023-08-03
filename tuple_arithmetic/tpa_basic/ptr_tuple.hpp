#include "defines.hpp"
#include "basics.hpp"
#include <type_traits>

#pragma once

TP_ENTER_NS
// ptr_tuple: wrap a pointer as an std::array
template<typename T, size_t N>
class ptr_tuple {

    public:
        FORCE_INLINE ptr_tuple() : m_data(nullptr) {}

        // Init from raw pointer
        FORCE_INLINE ptr_tuple(T* data) : m_data(data) {}
        FORCE_INLINE ptr_tuple(T* data, std::ptrdiff_t offset) : m_data(data+offset) {}

        // Init from std::array
        template<size_t num>
            requires(N < num)
        FORCE_INLINE ptr_tuple(std::array<T, num> arr) : m_data(arr.data()) {}

        template<size_t num>
            requires(N < num)
        FORCE_INLINE ptr_tuple(std::array<T, num> arr, std::ptrdiff_t offset) : m_data(arr.data()+offset) {}

        // Init from other ptr_tuple
        template<size_t num>
            requires(N < num)
        FORCE_INLINE ptr_tuple(ptr_tuple<T, num> other) : m_data(other.data()) {}

        template<size_t num>
            requires(N < num)
        FORCE_INLINE ptr_tuple(ptr_tuple<T, num> other, std::ptrdiff_t offset) : m_data(other.data()+offset) {}

        FORCE_INLINE T* set_buf(T* data, std::ptrdiff_t offset=0) {
            return m_data = data + offset;
        }

        template<typename Idx>
        FORCE_INLINE T& operator[](Idx&& idx) {
            return m_data[idx];
        }

        template<typename Idx>
        FORCE_INLINE const T& operator[](Idx&& idx) const {
            return m_data[idx];
        }

        FORCE_INLINE T* data() {
            return m_data;
        }

        template<typename Ty>
            requires(std::convertible_to<Ty, T>)
        FORCE_INLINE auto operator=(Ty&& other) {
            constexpr_for<0, N, 1>( [this](auto I, Ty&& o) {
                constexpr auto i = decltype(I)::value;
                m_data[i] = o;
            }, std::forward<Ty>(other));
            return *this;
        }

        template<tuple_like Tp>
        FORCE_INLINE auto operator=(Tp&& other) {
            constexpr_for<0, N, 1>( [this](auto I, Tp&& o) {
                constexpr auto i = decltype(I)::value;
                m_data[i] = std::get<i>(o);
            }, std::forward<Tp>(other));
            return *this;
        }

    private:
        T* m_data;

};

template<typename T> struct is_ptr_tuple : public std::false_type {};
template<typename T, size_t N> struct is_ptr_tuple<ptr_tuple<T, N>> : public std::true_type {};
template<typename T> static constexpr bool is_ptr_tuple_v = is_ptr_tuple<T>::value;

template<size_t N, typename T>
FORCE_INLINE auto make_ptr_tuple(T* data) { return ptr_tuple<T, N>(data); }
template<size_t N, typename T>
FORCE_INLINE auto make_ptr_tuple(T* data, std::ptrdiff_t offset) { return ptr_tuple<T, N>(data, offset); }

template<size_t N, typename T, size_t num>
FORCE_INLINE auto make_ptr_tuple(std::array<T, num> data) { return ptr_tuple<T, N>(data); }
template<size_t N, typename T, size_t num>
FORCE_INLINE auto make_ptr_tuple(std::array<T, num> data, std::ptrdiff_t offset) { return ptr_tuple<T, N>(data, offset); }

TP_EXIT_NS

namespace std {
    template<typename T, size_t N>
    struct tuple_size<TP_IN_NS(ptr_tuple)<T, N>> : public integral_constant<size_t, N> {};

    template<size_t idx, typename T, size_t N>
    struct tuple_element<idx, TP_IN_NS(ptr_tuple)<T, N>> {
        using type = T;
    };

    template<size_t idx, typename T, size_t N>
        requires (idx < N)
    FORCE_INLINE constexpr T& get(TP_IN_NS(ptr_tuple)<T, N>& pt) {
        return pt[idx];
    }

    template<size_t idx, typename T, size_t N>
        requires (idx < N)
    FORCE_INLINE constexpr const T& get(const TP_IN_NS(ptr_tuple)<T, N>& pt) {
        return pt[idx];
    }
}
