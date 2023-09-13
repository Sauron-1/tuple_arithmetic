#include <tuple_arithmetic.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_session.hpp>

#include <xsimd/xsimd.hpp>

#include <iostream>

using namespace std;
using namespace tpa;

using simd_t = xsimd::make_sized_batch_t<double, 4>;
using isimd_t = xsimd::make_sized_batch_t<int64_t, 4>;

TEST_CASE( "simd and array conversion", "[simd array convert]" ) {

    SECTION( "simd type to array" ) {
        simd_t simd{1, 2, 3, 4};
        auto a = tpa::to_array(simd);
        REQUIRE(std::is_same_v<decltype(a), std::array<double, 4>>);
        auto ps = to_array(simd);
        REQUIRE(a[0] == ps[0]);
        REQUIRE(a[1] == ps[1]);
        REQUIRE(a[2] == ps[2]);
        REQUIRE(a[3] == ps[3]);
    }

    SECTION( "tuple_like to simd type" ) {
        std::tuple a{1, 2.0, 3ul, 4.1f};
        auto simd = tpa::to_simd<double>(a);
        REQUIRE(std::is_same_v<decltype(simd), simd_t>);
        auto isimd = tpa::to_simd<int64_t>(a);
        REQUIRE(std::is_same_v<decltype(isimd), isimd_t>);
        auto pis = to_array(isimd);
        REQUIRE(pis[0] == 1ll);
        REQUIRE(pis[1] == 2ll);
        REQUIRE(pis[2] == 3ll);
        REQUIRE(pis[3] == 4ll);
    }

}

TEST_CASE( "simd assign", "[simd assign]" ) {

    SECTION( "value assign to simd" ) {
        simd_t simd;
        tpa::assign(simd, 1);
        auto ps = to_array(simd);
        for (int i = 0; i < 4; ++i) REQUIRE(ps[i] == 1.0);

        tpa::assign(simd, simd_t{2});
        ps = to_array(simd);
        for (int i = 0; i < 4; ++i) REQUIRE(ps[i] == 2.0);

        std::tuple arr{3, 3.0, 3ul, 3.1f};
        tpa::assign(simd, arr);
        ps = to_array(simd);
        for (int i = 0; i < 3; ++i) REQUIRE(ps[i] == 3.0);
        REQUIRE(ps[3] == 3.1f);
    }

    SECTION(" simd assign to tuple-like" ) {
        isimd_t isimd{1};
        std::tuple<int, double, float, size_t> tp;
        tpa::assign(tp, isimd);
        REQUIRE(std::get<0>(tp) == 1);
        REQUIRE(std::get<1>(tp) == 1.0);
        REQUIRE(std::get<2>(tp) == 1.0f);
        REQUIRE(std::get<3>(tp) == 1ull);
    }

    SECTION( "assign to array of simd" ) {
        std::tuple<simd_t, isimd_t> tp;
        tpa::assign(tp, tuple{isimd_t{1}, tuple{1, 2.0, 3ull, 4.1f}});
        auto ps = to_array(get<0>(tp));
        auto pis = to_array(get<1>(tp));
        for (int i = 0; i < 4; ++i) REQUIRE(ps[i] == 1.0);
        REQUIRE(pis[0] == 1);
        REQUIRE(pis[1] == 2);
        REQUIRE(pis[2] == 3);
        REQUIRE(pis[3] == 4);
    }

    SECTION( "assign value to array of simd" ) {
        std::tuple<simd_t, isimd_t> tp;
        tpa::assign(tp, 2);
        auto ps = to_array(get<0>(tp));
        auto pis = to_array(get<1>(tp));
        for (int i = 0; i < 4; ++i) REQUIRE(ps[i] == 2.0);
        for (int i = 0; i < 4; ++i) REQUIRE(pis[i] == 2);
    }

    SECTION( "assign to array of tuple_like" ) {
        std::tuple<simd_t, std::tuple<int, size_t, double, float>> tp;
        tpa::assign(tp, std::tuple{std::tuple{1, 2.0, 3ull, 4.0f}, isimd_t{1}});
        auto ps = to_array(get<0>(tp));
        REQUIRE(ps[0] == 1);
        REQUIRE(ps[1] == 2);
        REQUIRE(ps[2] == 3);
        REQUIRE(ps[3] == 4);

        REQUIRE(get<0>(get<1>(tp)) == 1);
        REQUIRE(get<1>(get<1>(tp)) == 1);
        REQUIRE(get<2>(get<1>(tp)) == 1);
        REQUIRE(get<3>(get<1>(tp)) == 1);
    }

    SECTION( "assign simds to array of tuple_like" ) {
        std::tuple<std::array<double, 4>, std::array<int, 4>> tp;
        tpa::assign(tp, std::array<simd_t, 2>{simd_t{1}, simd_t{2}});
        for (int i = 0; i < 4; ++i) REQUIRE(get<0>(tp)[i] == 1);
        for (int i = 0; i < 4; ++i) REQUIRE(get<1>(tp)[i] == 2);
    }

}
