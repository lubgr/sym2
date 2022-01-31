
#include "constants.h"
#include "doctest/doctest.h"
#include "expr.h"
#include "exprliteral.h"
#include "get.h"

using namespace sym2;
using namespace std::literals::string_view_literals;

TEST_CASE("Scalar retrieval")
{
    const Expr si{42};
    const Expr sr{7, 11};
    const LargeInt largeInt{"8233298749837489247029730960165010709217309487209740928934928"};
    const Expr li{LargeIntRef{largeInt}};

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
        CHECK(get<LargeInt>(si) == LargeInt{42});
    }

    SUBCASE("Get large positive integer")
    {
        CHECK(get<LargeInt>(li) == largeInt);
    }

    SUBCASE("Get large negative integer")
    {
        const Expr nli{LargeIntRef{-largeInt}};

        CHECK(get<LargeInt>(nli) == -largeInt);
    }

    SUBCASE("Get large rational from small int/rational")
    {
        CHECK(get<LargeRational>(42_ex) == LargeRational{42, 1});
        CHECK(get<LargeRational>(sr) == LargeRational{7, 11});
    }

    SUBCASE("Get large rational")
    {
        const LargeRational largeRational{LargeInt{3}, largeInt};

        CHECK(get<LargeRational>(Expr{LargeIntRef{largeInt}}) == LargeRational{largeInt, 1});
        CHECK(get<LargeRational>(Expr{LargeRationalRef{largeRational}}) == largeRational);
    }

    SUBCASE("Get double from numeric types")
    {
        CHECK(get<double>(42_ex) == doctest::Approx(42.0));
        CHECK(get<double>(Expr{7, 11}) == doctest::Approx(7.0 / 11.0));
        CHECK(get<double>(Expr{LargeIntRef{largeInt}}) == doctest::Approx(static_cast<double>(largeInt)));

        const LargeRational largeRational{LargeInt{123456789}, largeInt};
        const double expected = 123456789.0 / static_cast<double>(largeInt);
        CHECK(get<double>(Expr{LargeRationalRef{largeRational}}) == doctest::Approx(expected));
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
