#include <tuple_arithmetic.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_session.hpp>
#include <type_traits>

TEST_CASE( "binary operators", "[binary op]" ) {
    auto a = std::make_tuple(1, 2.0);
    auto b = std::array<float, 2>{3, 4};

    SECTION( "tuple tuple add" ) {
        auto c = a + b;
        REQUIRE(get<0>(c) == 4.0);
        REQUIRE(std::is_same_v<std::tuple_element_t<0, decltype(c)>, float>);
        REQUIRE(get<1>(c) == 6.0);
        REQUIRE(std::is_same_v<std::tuple_element_t<1, decltype(c)>, double>);
    }

    SECTION( "tuple value add" ) {
        auto c = a + 1;
        REQUIRE(get<0>(c) == 2);
        REQUIRE(std::is_same_v<std::tuple_element_t<0, decltype(c)>, int>);
        REQUIRE(get<1>(c) == 3.0);
        REQUIRE(std::is_same_v<std::tuple_element_t<1, decltype(c)>, double>);
    }

    SECTION( "dot" ) {
        REQUIRE( tpa::dot(a, b) == 1*3.0f + 2.0*4.0f);
    }

    SECTION( "cross2" ) {
        REQUIRE( tpa::cross(a, b) == 1*4.0f - 2.0*3.0f);
    }

    SECTION( "cross3" ) {
        auto a1 = std::make_tuple(1.0, 2.0, 3.0);
        auto b1 = std::array<double, 3>{4, 5, 6};
        auto c = tpa::cross(a1, b1);
        REQUIRE(c[0] == 2.0*6.0 - 3.0*5.0);
        REQUIRE(c[1] == 3.0*4.0 - 1.0*6.0);
        REQUIRE(c[2] == 1.0*5.0 - 2.0*4.0);
    }

    SECTION( "index" ) {
        auto tp = std::make_tuple(
                std::array<int, 2>{1, 2},
                std::array<float, 2>{3, 4});
        auto r1 = tpa::index(tp, std::array{0, 1});
        REQUIRE( get<0>(r1) == get<0>(tp)[0] );
        REQUIRE( get<1>(r1) == get<1>(tp)[1] );

        auto r2 = tpa::index(tp, 1);
        REQUIRE( get<0>(r2) == get<0>(tp)[1] );
        REQUIRE( get<1>(r2) == get<1>(tp)[1] );
    }

    SECTION( "invoke" ) {
        auto tp = std::make_tuple(
                [](int a) { return a; },
                [](int a) { return a+1; });
        auto r1 = tpa::invoke(tp, std::array{1, 0});
        REQUIRE( get<0>(r1) == get<0>(tp)(1) );
        REQUIRE( get<1>(r1) == get<1>(tp)(0) );

        auto r2 = tpa::invoke(tp, 1);
        REQUIRE( get<0>(r2) == get<0>(tp)(1) );
        REQUIRE( get<1>(r2) == get<1>(tp)(1) );
    }
}
