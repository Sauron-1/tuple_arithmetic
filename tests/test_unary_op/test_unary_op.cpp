#include <tuple_arithmetic.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_session.hpp>
#include <type_traits>

TEST_CASE( "unary operators", "[unary op]" ) {
    auto a = std::make_tuple(1, 2.0);
    SECTION( "negative" ) {
        auto b = -a;
        REQUIRE( get<0>(b) == - get<0>(a));
        REQUIRE( get<1>(b) == - get<1>(a));
    }

    SECTION( "cast" ) {
        auto b = tpa::cast<float>(a);
        REQUIRE( std::is_same_v<float, std::tuple_element_t<0, decltype(b)>> );
        REQUIRE( std::is_same_v<float, std::tuple_element_t<1, decltype(b)>> );
    }

    SECTION( "firstN" ) {
        auto b = tpa::firstN<1>(a);
        REQUIRE( std::tuple_size_v<decltype(b)> == 1 );
        REQUIRE( get<0>(a) == get<0>(b) );
    }
}
