
#include "abc.h"
#include "canonical.h"
#include "doctest/doctest.h"
#include "expr.h"

using namespace sym2;

const Expr n{42};
const Expr s = sum(42, "a", "b");
const Expr pr = product(42, "a", "b");
const Expr pw = power(42, "a");

TEST_CASE("Identity equality")
{
    for (ExprView e : {n, a, pi, s, pr, pw})
        CHECK(e == e);
}

TEST_CASE("Scalar Equality by value")
{
    CHECK(Expr{"a"} == a);
    CHECK(42 == n);
    CHECK(Expr{0} == 0);
    CHECK(Expr{1} == 1);
}

TEST_CASE("Composite equality")
{
    // const Expr s = sum(42, "a", "b");
    // const Expr pr = product(42, "a", "b");
    // const Expr pw = power(42, "a");
}
