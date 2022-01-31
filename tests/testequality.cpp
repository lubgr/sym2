
#include "autosimpl.h"
#include "constants.h"
#include "doctest/doctest.h"
#include "expr.h"
#include "exprliteral.h"

using namespace sym2;

TEST_CASE("Equality")
{
    const Expr s = autoSum(42_ex, "a"_ex, "b"_ex);
    const Expr pr = autoProduct(42_ex, "a"_ex, "b"_ex);
    const Expr pw = autoPower(42_ex, "a"_ex);

    SUBCASE("Identity equality")
    {
        for (ExprView<> e : {Expr{42}, Expr{"a"}, pi, s, pr, pw})
            CHECK(e == e);
    }

    SUBCASE("Scalar Equality by value")
    {
        CHECK(Expr{"a"} == "a"_ex);
        CHECK(Expr{42} == 42_ex);
        CHECK(Expr{0} == 0_ex);
        CHECK(Expr{1} == 1_ex);
    }

    SUBCASE("Composite equality")
    {
        CHECK(s == s);
        CHECK(pr == pr);
        CHECK(pw == pw);

        // TODO complex cases
    }

    SUBCASE("Scalar inequality")
    {
        CHECK(Expr{"a"} != "b"_ex);
        CHECK(Expr{42} != 43_ex);
    }
}
