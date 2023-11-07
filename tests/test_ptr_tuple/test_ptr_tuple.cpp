#include <tuple_arithmetic.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_session.hpp>
#include <type_traits>

TEST_CASE( "ptr tuple", "[ptr tuple]" ) {
    double *ptr = new double[3];
    auto pt = tpa::make_ptr_tuple<3>(ptr);

    SECTION( "assign with value" ) {
        pt = 2;
        pt[1] = 3;
        REQUIRE(pt[0] == 2.0);
        REQUIRE(pt[1] == 3.0);
        REQUIRE(pt[2] == 2.0);
    }

    SECTION( "assign with tuple" ) {
        tpa::assign(pt, std::make_tuple(1, 2.0, 3.0f));
        REQUIRE(pt[0] == 1.0);
        REQUIRE(pt[1] == 2.0);
        REQUIRE(pt[2] == 3.0);
    }

    SECTION( "avaliable for arithmetic ops" ) {
        pt = 1;
        auto tmp = pt * 2;
        REQUIRE(tmp[0] == 2.0);
        REQUIRE(get<2>(tmp) == 2.0);
    }
}
