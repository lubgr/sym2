
#include "canonical.h"
#include "doctest/doctest.h"
#include "query.h"

using namespace sym2;

TEST_CASE("Common queries")
{
    const Expr pi{"pi"};
    const Expr euler{"euler"};
    const Expr a{"a"};
    const Expr b{"b"};
    const Expr n{42};
    const Expr s = sum(42, "a", "b");
    const Expr pr = product(42, "a", "b");
    const Expr pw = power(42, "a");

    SUBCASE("Scalar")
    {
        for (ExprView e : {pi, euler, a, b, n})
            CHECK(isScalar(e));

        for (ExprView e : {pw})
            CHECK_FALSE(isScalar(e));
    }

    SUBCASE("Number")
    {
        CHECK(isNumber(n));

        for (ExprView e : {pi, euler, a, b, s, pr, pw})
            CHECK_FALSE(isNumber(e));
    }

    SUBCASE("Constant")
    {
        CHECK(isConstant(pi));
        CHECK(isConstant(euler));

        for (ExprView e : {n, a, b, s, pr, pw})
            CHECK_FALSE(isConstant(e));
    }

    SUBCASE("Number of operands")
    {
        for (ExprView e : {pi, euler, a, b, n})
            CHECK(nOps(e) == 0);

        CHECK(nOps(s) == 3);
        CHECK(nOps(pr) == 3);
        CHECK(nOps(pw) == 2);
    }
}
