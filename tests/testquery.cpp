
#include "autosimpl.h"
#include "constants.h"
#include "doctest/doctest.h"
#include "query.h"
#include "trigonometric.h"

using namespace sym2;

TEST_CASE("Counters")
{
    const Expr fp{3.14};
    const Expr sr{7, 11};
    const LargeInt largeInt{"2323498273984729837498234029380492839489234902384"};
    const Expr li{largeInt};
    const Expr lr{LargeRational{LargeInt{"1234528973498279834827384284"}, largeInt}};
    const Expr cx = cpx(2_ex, 3_ex);
    const Expr s = sum(42_ex, "a"_ex, "b"_ex);
    const Expr pr = product(42_ex, "a"_ex, "b"_ex);
    const Expr pw = power(42_ex, "a"_ex);
    const Expr sinA = sin("a"_ex);
    const Expr atan2Ab = atan2("a"_ex, "b"_ex);

    SUBCASE("Number of operands")
    {
        for (ExprView e : {"a"_ex, 42_ex, fp, sr, pi, euler, lr})
            CHECK(nLogicalOperands(e) == 0);

        CHECK(nLogicalOperands(cx) == 2);
        CHECK(nLogicalOperands(s) == 3);
        CHECK(nLogicalOperands(pr) == 3);
        CHECK(nLogicalOperands(pw) == 2);
        CHECK(nLogicalOperands(sinA) == 1);
        CHECK(nLogicalOperands(atan2Ab) == 2);
    }

    SUBCASE("Number of child blobs")
    {
        const auto composite = sum(sinA, product(pw, s), pr, cx, lr, atan2Ab);

        for (ExprView e : {"a"_ex, 42_ex, fp, sr, pi, euler, li, lr, cx})
            CHECK(nChildBlobs(e) == e.size() - 1);
    }
}

TEST_CASE("Nth operand queries")
{
    SUBCASE("Sum with product")
    {
        const Expr pr = product(10_ex, "b"_ex, "c"_ex);
        const Expr s = sum(42_ex, "a"_ex, pr, "d"_ex);

        CHECK(first(s) == 42_ex);
        CHECK(second(s) == "a"_ex);
        CHECK(nth(s, 3) == pr);
        CHECK(nth(s, 4) == "d"_ex);
    }

    SUBCASE("Large Rational")
    {
        const LargeInt denom{"1234528973498279834827384284"};
        const Expr lr{LargeRational{1, denom}};

        CHECK(first(lr) == 1_ex);
        CHECK(second(lr) == Expr{denom});
    }

    SUBCASE("Complex number")
    {
        const Expr cx = cpx(2_ex, 3_ex);

        CHECK(first(cx) == 2_ex);
        CHECK(second(cx) == 3_ex);
    }
}
