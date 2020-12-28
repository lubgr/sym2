
#include "constants.h"
#include "doctest/doctest.h"
#include "expr.h"
#include "get.h"

using namespace sym2;
using namespace std::literals::string_view_literals;

TEST_CASE("Scalar retrieval")
{
    const Expr si{42};
    const Expr sr{7, 11};
    const Int largeInt{"8233298749837489247029730960165010709217309487209740928934928"};
    const Expr li{largeInt};

    SUBCASE("Get small integer")
    {
        CHECK(get<int>(si) == 42);
    }

    SUBCASE("Get small rational from small integer")
    {
        CHECK(get<SmallRational>(si).num == 42);
        CHECK(get<SmallRational>(si).denom == 1);
    }

    SUBCASE("Get small rational")
    {
        CHECK(get<SmallRational>(sr).num == 7);
        CHECK(get<SmallRational>(sr).denom == 11);
    }

    SUBCASE("Get large integer from small integer")
    {
        CHECK(get<Int>(si) == Int{42});
    }

    SUBCASE("Get large positive integer")
    {
        CHECK(get<Int>(li) == largeInt);
    }

    SUBCASE("Get large negative integer")
    {
        const Expr nli{-largeInt};

        CHECK(get<Int>(nli) == -largeInt);
    }

    SUBCASE("Get large rational from small int/rational")
    {
        CHECK(get<Rational>(42_ex) == Rational{42, 1});
        CHECK(get<Rational>(sr) == Rational{7, 11});
    }

    SUBCASE("Get large rational")
    {
        const Rational largeRational{Int{3}, largeInt};

        CHECK(get<Rational>(Expr{largeInt}) == Rational{largeInt, 1});
        CHECK(get<Rational>(Expr{largeRational}) == largeRational);
    }

    SUBCASE("Get double from numeric types")
    {
        CHECK(get<double>(42_ex) == doctest::Approx(42.0));
        CHECK(get<double>(Expr{7, 11}) == doctest::Approx(7.0 / 11.0));
        CHECK(get<double>(Expr{largeInt}) == doctest::Approx(static_cast<double>(largeInt)));

        const Rational largeRational{Int{123456789}, largeInt};
        const double expected = 123456789.0 / static_cast<double>(largeInt);
        CHECK(get<double>(Expr{largeRational}) == doctest::Approx(expected));
    }

    SUBCASE("Get double from nuerically evaluable expression")
    {
        // TODO
    }

    SUBCASE("Get symbol name")
    {
        const auto a = "a"_ex;
        const auto longer = "abc_{def}"_ex;

        CHECK(get<std::string_view>(a) == "a"sv);
        CHECK(get<std::string_view>(longer) == "abc_{def}"sv);
    }

    SUBCASE("Get value and name of constant")
    {
        CHECK(get<std::string_view>(pi) == "pi"sv);
        CHECK(get<std::string_view>(euler) == "e"sv);

        CHECK(get<double>(pi) == doctest::Approx(2.0 * std::acos(0.0)));
        CHECK(get<double>(euler) == doctest::Approx(std::exp(1)));
    }
}
