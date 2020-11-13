
#include "abc.h"
#include "canonical.h"
#include "doctest/doctest.h"
#include "expr.h"

using namespace sym2;

const Expr s = sum(42, "a", "b");
const Expr pr = product(42, "a", "b");
const Expr pw = power(42, "a");

TEST_CASE("Identity equality")
{
    for (ExprView e : {42_ex, a, pi, s, pr, pw})
        CHECK(e == e);
}

TEST_CASE("Scalar Equality by value")
{
    CHECK("a"_ex == a);
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
