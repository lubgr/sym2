
#include "canonical.h"
#include "constants.h"
#include "doctest/doctest.h"
#include "expr.h"

using namespace sym2;

const Expr s = sum(42_ex, "a"_ex, "b"_ex);
const Expr pr = product(42_ex, "a"_ex, "b"_ex);
const Expr pw = power(42_ex, "a"_ex);

TEST_CASE("Identity equality")
{
    for (ExprView e : {42_ex, "a"_ex, pi, s, pr, pw})
        CHECK(e == e);
}

TEST_CASE("Scalar Equality by value")
{
    CHECK("a"_ex == "a"_ex);
    CHECK(42_ex == 42_ex);
    CHECK(0_ex == 0_ex);
    CHECK(1_ex == 1_ex);
}

TEST_CASE("Composite equality")
{
    CHECK(s == s);
    CHECK(pr == pr);
    CHECK(pw == pw);

    // TODO complex cases
}

TEST_CASE("Scalar inequality")
{
    CHECK("a"_ex != "b"_ex);
    CHECK(42_ex != 43_ex);
}
