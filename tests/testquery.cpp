
#include "abc.h"
#include "canonical.h"
#include "doctest/doctest.h"
#include "query.h"

using namespace sym2;

TEST_CASE("Common queries")
{
    const Expr fp{3.14};
    const Expr sr{7, 11};
    const Int largeInt{"2323498273984729837498234029380492839489234902384"};
    const Expr li{largeInt};
    const Expr lr{Rational{Int{"1234528973498279834827384284"}, largeInt}};
    const Expr s = sum(42, "a", "b");
    const Expr pr = product(42, "a", "b");
    const Expr pw = power(42, "a");

    SUBCASE("Numeric subtypes")
    {
        CHECK(isLargeInt(li));
        CHECK(isLargeRational(lr));

        CHECK(isSmallInt(42_ex));
        CHECK(isSmallRational(sr));
        CHECK(isDouble(fp));
    }

    SUBCASE("Scalar")
    {
        for (ExprView e : {42_ex, d, a, b, euler, pi})
            CHECK(isScalar(e));

        for (ExprView e : {pw})
            CHECK_FALSE(isScalar(e));
    }

    SUBCASE("Number")
    {
        for (ExprView number : {42_ex, fp, sr, li, lr})
            CHECK(isNumber(number));

        for (ExprView e : {pi, euler, a, b, s, pr, pw})
            CHECK_FALSE(isNumber(e));
    }

    SUBCASE("Integer")
    {
        for (ExprView number : {42_ex, Expr{li}})
            CHECK(isInteger(number));

        for (ExprView e : {pi, euler, a, b, s, pr, pw, fp, sr, lr})
            CHECK_FALSE(isInteger(e));
    }

    SUBCASE("Constant")
    {
        CHECK(isConstant(pi));
        CHECK(isConstant(euler));

        for (ExprView e : {42_ex, fp, sr, lr, a, b, s, pr, pw})
            CHECK_FALSE(isConstant(e));
    }

    SUBCASE("Symbol")
    {
        for (ExprView e : {a, b, c})
            CHECK(isSymbol(e));

        for (ExprView e : {42_ex, fp, sr, lr, pi, euler, s, pr, pw})
            CHECK_FALSE(isSymbol(e));
    }

    SUBCASE("Symbol or constant")
    {
        for (ExprView e : {a, b, c, pi, euler})
            CHECK(isSymbolOrConstant(e));

        for (ExprView e : {42_ex, fp, sr, lr, s, pr, pw})
            CHECK_FALSE(isSymbolOrConstant(e));
    }

    SUBCASE("Number of operands")
    {
        for (ExprView e : {a, b, 42_ex, fp, sr})
            CHECK(nOps(e) == 0);

        CHECK(nOps(pi) == 1);
        CHECK(nOps(euler) == 1);

        CHECK(nOps(lr) == 2);
        CHECK(nOps(s) == 3);
        CHECK(nOps(pr) == 3);
        CHECK(nOps(pw) == 2);
    }
}
