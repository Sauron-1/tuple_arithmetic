#include <tuple_arithmetic.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_session.hpp>
#include <type_traits>

using namespace std;
using namespace tpa;

TEST_CASE( "unary operators", "[unary op]" ) {
    auto a = make_tuple(1, 2.0);
    SECTION( "negative" ) {
        auto b = -a;
        REQUIRE( get<0>(b) == - get<0>(a));
        REQUIRE( get<1>(b) == - get<1>(a));
    }

    SECTION( "cast" ) {
        auto b = cast<float>(a);
        REQUIRE( is_same_v<float, tuple_element_t<0, decltype(b)>> );
        REQUIRE( is_same_v<float, tuple_element_t<1, decltype(b)>> );
    }

    SECTION( "firstN" ) {
        auto b = firstN<1>(a);
        REQUIRE( tuple_size_v<decltype(b)> == 1 );
        REQUIRE( get<0>(a) == get<0>(b) );
    }
}
