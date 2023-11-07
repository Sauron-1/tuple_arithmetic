#include <tuple_arithmetic.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_session.hpp>
#include <type_traits>

TEST_CASE( "ternary operators", "[ternary op]" ) {
    auto a = std::make_tuple(1, 5.0);
    auto b = std::array<float, 2>{3, 4};

    SECTION( "Selection" ) {
        auto c = tpa::select(std::make_tuple(0, true), a, b);
        REQUIRE( get<0>(c) == get<0>(b) );
        REQUIRE( get<1>(c) == get<1>(a) );
    }
}
