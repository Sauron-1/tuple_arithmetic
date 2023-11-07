#include <tuple_arithmetic.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_session.hpp>
#include <type_traits>

TEST_CASE( "const tuple", "[const tuple]" ) {
    auto a = tpa::const_tuple<int, 4>{1};
    const auto ca = tpa::const_tuple<double, 4>{2};

    REQUIRE( std::tuple_size_v<decltype(a)> == 4 );
    REQUIRE( std::tuple_size_v<decltype(ca)> == 4 );
    REQUIRE( get<3>(a) == get<0>(a) );
    REQUIRE( get<3>(ca) == get<0>(ca) );
    REQUIRE( a[0] == a[3] );
    REQUIRE( ca[0] == ca[3] );

    auto b = a + 5;
    REQUIRE( std::tuple_size_v<decltype(b)> == 4 );
    REQUIRE( get<3>(a) == get<0>(a) );
}
