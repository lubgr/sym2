
#include "autosimpl.h"
#include "constants.h"
#include "doctest/doctest.h"
#include "expr.h"

using namespace sym2;

TEST_CASE("Equality")
{
    const Expr s = autoSum(42_ex, "a"_ex, "b"_ex);
    const Expr pr = autoProduct(42_ex, "a"_ex, "b"_ex);
    const Expr pw = autoPower(42_ex, "a"_ex);

    SUBCASE("Identity equality")
    {
        for (ExprView<> e : {42_ex, "a"_ex, pi, s, pr, pw})
            CHECK(e == e);
    }

    SUBCASE("Scalar Equality by value")
    {
        CHECK("a"_ex == "a"_ex);
        CHECK(42_ex == 42_ex);
        CHECK(0_ex == 0_ex);
        CHECK(1_ex == 1_ex);
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
        CHECK("a"_ex != "b"_ex);
        CHECK(42_ex != 43_ex);
    }
}
