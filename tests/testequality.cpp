
#include <array>
#include "doctest/doctest.h"
#include "sym2/constants.h"
#include "sym2/expr.h"
#include "testutils.h"

using namespace sym2;

TEST_CASE("Equality")
{
    const Expr::allocator_type alloc{};
    const Expr s = directSum({42_ex, "a"_ex, "b"_ex}, alloc);
    const Expr pr = directProduct({42_ex, "a"_ex, "b"_ex}, alloc);
    const Expr pw = directPower(42_ex, "a"_ex, alloc);

    SUBCASE("Identity equality")
    {
        const FixedExpr<1> n = 42_ex;
        const FixedExpr<1> a = "a"_ex;

        for (ExprView<> e : std::array<ExprView<>, 6>{n, a, pi, s, pr, pw})
            CHECK(e == e);
    }

    SUBCASE("Scalar Equality by value")
    {
        CHECK_EQ(Expr{"a", alloc}, "a"_ex);
        CHECK_EQ(Expr{42, alloc}, 42_ex);
        CHECK_EQ(Expr{0, alloc}, 0_ex);
        CHECK_EQ(Expr{1, alloc}, 1_ex);
    }

    SUBCASE("Composite equality")
    {
        CHECK_EQ(s, s);
        CHECK_EQ(pr, pr);
        CHECK_EQ(pw, pw);

        // TODO complex cases
    }

    SUBCASE("Scalar inequality")
    {
        CHECK_NE(Expr{"a", alloc}, "b"_ex);
        CHECK_NE(Expr{42, alloc}, 43_ex);
    }
}
