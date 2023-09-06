#include <bits/utility.h>
#include <tuple_arithmetic/tuple_arithmetic.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_session.hpp>

#include <xsimd/xsimd.hpp>

#include <iostream>

using namespace std;
using namespace tpa;

using simd_t = xsimd::make_sized_batch_t<double, 4>;
using isimd_t = xsimd::make_sized_batch_t<int64_t, 4>;

TEST_CASE( "simd binary op", "[simd binary op]" ) {

    SECTION( "simd and array op" ) {
        simd_t simd{1, 2, 3, 4};
        auto a = simd + std::tuple{1, 2.0, 3ull, 4.0f};
        REQUIRE(xsimd::is_batch<decltype(a)>::value);
        auto arr = to_array(a);
        REQUIRE(arr[0] == 2);
        REQUIRE(arr[1] == 4);
        REQUIRE(arr[2] == 6);
        REQUIRE(arr[3] == 8);
    }

    SECTION( "array of simd op" ) {
        auto tp1 = std::tuple{simd_t{1}, std::tuple{1, 2.0, 3ull, 4.0f}};
        auto tp2 = simd_t{2};
        auto r1 = tp1 + tp2;
        REQUIRE(xsimd::is_batch<std::tuple_element_t<0, decltype(r1)>>::value);
        REQUIRE(xsimd::is_batch<std::tuple_element_t<1, decltype(r1)>>::value);
        REQUIRE(to_array(get<0>(r1))[0] == 3);
        REQUIRE(to_array(get<1>(r1))[1] == 4);

        auto r2 = tp1 + std::tuple{isimd_t{1}, simd_t{2}};
        REQUIRE(xsimd::is_batch<std::tuple_element_t<0, decltype(r2)>>::value);
        REQUIRE(xsimd::is_batch<std::tuple_element_t<1, decltype(r2)>>::value);
        REQUIRE(to_array(get<0>(r2))[0] == 2);
        REQUIRE(to_array(get<1>(r2))[1] == 4);
    }

    SECTION( "select" ) {
        simd_t simd{1, 2, 3, 4};
        auto r1 = select(simd < 3, 0.0, isimd_t{5});
        REQUIRE(xsimd::is_batch<decltype(r1)>::value);
        REQUIRE(std::is_same_v<typename decltype(r1)::value_type, double>);
        auto arr1 = to_array(r1);
        REQUIRE(arr1[0] == 0);
        REQUIRE(arr1[1] == 0);
        REQUIRE(arr1[2] == 5);
        REQUIRE(arr1[3] == 5);
    }

}
