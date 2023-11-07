#include <tuple_arithmetic.hpp>
#include <tuple_math.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_session.hpp>
#include <type_traits>

TEST_CASE( "binary operators", "[binary op]" ) {
    auto a = std::make_tuple(1, 4.0);
    const auto b = std::array<float, 2>{3, 2};

    SECTION( "tuple tuple compare" ) {
        using tpa::operator>;
        auto c = a > b;
        REQUIRE(not get<0>(c));
        REQUIRE(get<1>(c));
    }

    SECTION( "tuple value compare" ) {
        using tpa::operator>;
        auto c = a > 2;
        REQUIRE(not get<0>(c));
        REQUIRE(get<1>(c));
        auto d = b > size_t(2);
        REQUIRE(get<0>(d));
        REQUIRE(not get<1>(d));
    }

}
