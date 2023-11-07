#include <tuple_arithmetic.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_session.hpp>

TEST_CASE( "value tuple assignment", "[tuple assignment]" ) {
    auto a = std::make_tuple(1, 2.0);

    SECTION( "assign with another tuple" ) {
        tpa::assign(a, std::make_tuple(2, 3));
        REQUIRE(get<0>(a) == 2);
        REQUIRE(get<1>(a) == 3.0);
    }

    SECTION( "assign with a value" ) {
        tpa::assign(a, 4);
        REQUIRE(get<0>(a) == 4);
        REQUIRE(get<1>(a) == 4.0);
    }
}

TEST_CASE( "reference tuple assignment", "[reference assignment]" ) {
    double a = 0.0;
    int b = 0;

    auto tp = std::tie(a, b);

    SECTION( "assign with a tuple" ) {
        tpa::assign(tp, std::array<int, 2>{2, 3});
        REQUIRE(a == 2.0);
        REQUIRE(b == 3);
    }

    SECTION( "assign with a value" ) {
        tpa::assign(tp, std::make_tuple(4, 5));
        REQUIRE(a == 4.0);
        REQUIRE(b == 5);
    }
}
