
#include "doctest/doctest.h"
#include "sym2/constants.h"
#include "sym2/expr.h"
#include "sym2/get.h"

using namespace sym2;
using namespace std::literals::string_view_literals;

TEST_CASE("Scalar retrieval")
{
    const Expr::allocator_type alloc{};
    const Expr si{42, alloc};
    const Expr sr{7, 11, alloc};
    const LargeInt largeInt{"8233298749837489247029730960165010709217309487209740928934928"};
    const Expr li{largeInt, alloc};

    SUBCASE("Get small integer")
    {
        CHECK(get<std::int16_t>(si) == 42);
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
        const Expr nli{LargeInt{-largeInt}, alloc};

        CHECK(get<LargeInt>(nli) == -largeInt);
    }

    SUBCASE("Get large rational from small int/rational")
    {
        CHECK(get<LargeRational>(42_ex) == LargeRational{42, 1});
        CHECK(get<LargeRational>(sr) == LargeRational{7, 11});
    }

    SUBCASE("Get large rational")
    {
        const LargeRational lr{LargeInt{3}, largeInt};

        CHECK(get<LargeRational>(Expr{largeInt, alloc}) == LargeRational{largeInt, 1});
        CHECK(get<LargeRational>(Expr{lr, alloc}) == lr);
    }

    SUBCASE("Get double from numeric types")
    {
        CHECK(get<double>(42_ex) == doctest::Approx(42.0));
        CHECK(get<double>(Expr{7, 11, alloc}) == doctest::Approx(7.0 / 11.0));
        CHECK(get<double>(Expr{largeInt, alloc}) == doctest::Approx(static_cast<double>(largeInt)));

        const LargeRational largeRational{LargeInt{123456789}, largeInt};
        const double expected = 123456789.0 / static_cast<double>(largeInt);
        CHECK(get<double>(Expr{largeRational, alloc}) == doctest::Approx(expected));
    }

    SUBCASE("Get double from nuerically evaluable expression")
    {
        // TODO
    }

    SUBCASE("Get symbol name")
    {
        const auto a = "a"_ex;
        const auto longer = "abc_d"_ex;

        CHECK(get<std::string_view>(a) == "a"sv);
        CHECK(get<std::string_view>(longer) == "abc_d"sv);
    }

    SUBCASE("Get value and name of constant")
    {
        CHECK(get<std::string_view>(pi) == "pi"sv);
        CHECK(get<std::string_view>(euler) == "e"sv);

        CHECK(get<double>(pi) == doctest::Approx(2.0 * std::acos(0.0)));
        CHECK(get<double>(euler) == doctest::Approx(std::exp(1)));
    }
}
