
#include "abc.h"
#include "canonical.h"
#include "constants.h"
#include "doctest/doctest.h"
#include "query.h"
#include "trigonometric.h"

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
    const Expr sinA = sin("a"_ex);
    const Expr atan2Ab = atan2("a"_ex, "b"_ex);

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

        for (ExprView e : {pw, pr, s, sinA, atan2Ab})
            CHECK_FALSE(isScalar(e));
    }

    SUBCASE("Number")
    {
        for (ExprView number : {42_ex, fp, sr, li, lr, cx})
            CHECK(isNumber(number));

        for (ExprView e : {pi, euler, a, b, s, pr, pw, sinA, atan2Ab})
            CHECK_FALSE(isNumber(e));
    }

    SUBCASE("Integer")
    {
        for (ExprView number : {42_ex, Expr{li}})
            CHECK(isInteger(number));

        for (ExprView e : {pi, euler, a, b, s, pr, pw, fp, sr, lr, cx, sinA, atan2Ab})
            CHECK_FALSE(isInteger(e));
    }

    SUBCASE("Constant")
    {
        CHECK(isConstant(pi));
        CHECK(isConstant(euler));

        for (ExprView e : {42_ex, fp, sr, lr, cx, a, b, s, pr, pw, sinA, atan2Ab})
            CHECK_FALSE(isConstant(e));
    }

    SUBCASE("Symbol")
    {
        for (ExprView e : {a, b, c})
            CHECK(isSymbol(e));

        for (ExprView e : {42_ex, fp, sr, lr, cx, pi, euler, s, pr, pw, sinA, atan2Ab})
            CHECK_FALSE(isSymbol(e));
    }

    SUBCASE("Symbol or constant")
    {
        for (ExprView e : {a, b, c, pi, euler})
            CHECK(isSymbolOrConstant(e));

        for (ExprView e : {42_ex, fp, sr, lr, cx, s, pr, pw, sinA, atan2Ab})
            CHECK_FALSE(isSymbolOrConstant(e));
    }

    SUBCASE("Sum")
    {
        CHECK(isSum(s));

        for (ExprView e : {42_ex, a, b, c, pi, fp, sr, lr, cx, pr, pw, sinA, atan2Ab})
            CHECK_FALSE(isSum(e));
    }

    SUBCASE("Product")
    {
        CHECK(isProduct(pr));

        for (ExprView e : {42_ex, a, b, c, pi, fp, sr, lr, cx, s, pw, sinA, atan2Ab})
            CHECK_FALSE(isProduct(e));
    }

    SUBCASE("Power")
    {
        CHECK(isPower(pw));

        for (ExprView e : {42_ex, a, b, c, pi, fp, sr, lr, cx, s, pr, sinA, atan2Ab})
            CHECK_FALSE(isPower(e));
    }

    SUBCASE("Function")
    {
        CHECK(isFunction(sinA));
        CHECK(isFunction(atan2Ab));
        CHECK(isFunction(sinA, "sin"));
        CHECK(isFunction(atan2Ab, "atan2"));

        CHECK_FALSE(isFunction(sinA, "cos"));

        for (ExprView e : {42_ex, a, b, c, pi, fp, sr, lr, cx, s, pr}) {
            CHECK_FALSE(isFunction(e));
            CHECK_FALSE(isFunction(e, "sin"));
        }
    }

    SUBCASE("Number of operands")
    {
        for (ExprView e : {a, b, 42_ex, fp, sr, pi, euler, lr})
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

        for (ExprView e : {a, b, 42_ex, fp, sr, pi, euler, li, lr, cx})
            CHECK(nChildBlobs(e) == e.size() - 1);
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
