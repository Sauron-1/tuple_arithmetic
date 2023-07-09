#include <tuple_arithmetic/tuple_arithmetic.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_session.hpp>
#include <type_traits>

using namespace std;
using namespace tpa;

TEST_CASE( "ternary operators", "[ternary op]" ) {
    auto a = make_tuple(1, 5.0);
    auto b = array<float, 2>{3, 4};

    SECTION( "Selection" ) {
        auto c = select(make_tuple(0, true), a, b);
        REQUIRE( get<0>(c) == get<0>(b) );
        REQUIRE( get<1>(c) == get<1>(a) );
    }
}
