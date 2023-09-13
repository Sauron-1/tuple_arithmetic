#include <tuple_arithmetic.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_session.hpp>
#include <type_traits>

using namespace std;
using namespace tpa;

TEST_CASE( "reduce operators", "[reduce op]" ) {
    auto a = make_tuple(1, 2.0, 3.0f, 4ull);

    SECTION( "reduce sum" ) {
        REQUIRE( tpa::sum(a) == 10.0 );
    }

    SECTION( "reduce prod" ) {
        REQUIRE( tpa::prod(a) == 24.0 );
    }

    auto b = make_tuple(1, 1.0, 2, 2.0);

    SECTION( "reduce min" ) {
        auto c = tpa::reduce_min(b);
        REQUIRE( c == 1.0 );
        REQUIRE( is_same_v<decltype(c), double> );
    }

    SECTION( "reduce max" ) {
        auto c = tpa::reduce_max(b);
        REQUIRE( c == 2.0 );
        REQUIRE( is_same_v<decltype(c), double> );
    }

    SECTION( "reduce all" ) {
        REQUIRE( tpa::all(make_tuple(true, 1, 1.0)) );
        REQUIRE( not tpa::all(make_tuple(false, true, true)) );
    }

    SECTION( "reduce any" ) {
        REQUIRE( tpa::any(make_tuple(true, false, false)) );
        REQUIRE( not tpa::any(make_tuple(false, 0, 0.0)) );
    }
}
