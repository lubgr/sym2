
#include "abc.h"
#include "canonical.h"
#include "constants.h"
#include "doctest/doctest.h"
#include "query.h"

using namespace sym2;

TEST_CASE("Type queries")
{
    const Expr fp{3.14};
    const Expr sr{7, 11};
    const Int largeInt{"2323498273984729837498234029380492839489234902384"};
    const Expr li{largeInt};
    const Expr lr{Rational{Int{"1234528973498279834827384284"}, largeInt}};
    const Expr cx = cpx(2, 3);
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

        for (ExprView n : {fp, sr, li, lr})
            CHECK(isRealNumber(n));

        CHECK(isComplexNumber(cx));
    }

    SUBCASE("Scalar")
    {
        for (ExprView e : {42_ex, cx, d, a, b, euler, pi})
            CHECK(isScalar(e));

        for (ExprView e : {pw})
            CHECK_FALSE(isScalar(e));
    }

    SUBCASE("Number")
    {
        for (ExprView number : {42_ex, fp, sr, li, lr, cx})
            CHECK(isNumber(number));

        for (ExprView e : {pi, euler, a, b, s, pr, pw})
            CHECK_FALSE(isNumber(e));
    }

    SUBCASE("Integer")
    {
        for (ExprView number : {42_ex, Expr{li}})
            CHECK(isInteger(number));

        for (ExprView e : {pi, euler, a, b, s, pr, pw, fp, sr, lr, cx})
            CHECK_FALSE(isInteger(e));
    }

    SUBCASE("Constant")
    {
        CHECK(isConstant(pi));
        CHECK(isConstant(euler));

        for (ExprView e : {42_ex, fp, sr, lr, cx, a, b, s, pr, pw})
            CHECK_FALSE(isConstant(e));
    }

    SUBCASE("Symbol")
    {
        for (ExprView e : {a, b, c})
            CHECK(isSymbol(e));

        for (ExprView e : {42_ex, fp, sr, lr, cx, pi, euler, s, pr, pw})
            CHECK_FALSE(isSymbol(e));
    }

    SUBCASE("Symbol or constant")
    {
        for (ExprView e : {a, b, c, pi, euler})
            CHECK(isSymbolOrConstant(e));

        for (ExprView e : {42_ex, fp, sr, lr, cx, s, pr, pw})
            CHECK_FALSE(isSymbolOrConstant(e));
    }

    SUBCASE("Number of operands")
    {
        for (ExprView e : {a, b, 42_ex, fp, sr, pi, euler})
            CHECK(nOps(e) == 0);

        CHECK(nOps(lr) == 2);
        CHECK(nOps(cx) == 2);
        CHECK(nOps(s) == 3);
        CHECK(nOps(pr) == 3);
        CHECK(nOps(pw) == 2);
    }
}

TEST_CASE("Nth operand queries")
{
    SUBCASE("Sum with product")
    {
        const Expr pr = product(10, "b", "c");
        const Expr s = sum(42, "a", pr, "d");

        CHECK(first(s) == 42_ex);
        CHECK(second(s) == "a"_ex);
        CHECK(nth(s, 3) == pr);
        CHECK(nth(s, 4) == "d"_ex);
    }

    SUBCASE("Large Rational")
    {
        const Int denom{"1234528973498279834827384284"};
        const Expr lr{Rational{1, denom}};

        CHECK(first(lr) == 1_ex);
        CHECK(second(lr) == Expr{denom});
    }

    SUBCASE("Complex number")
    {
        const Expr cx = cpx(2, 3);

        CHECK(first(cx) == 2_ex);
        CHECK(second(cx) == 3_ex);
    }
}
