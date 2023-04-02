
#include <array>
#include "doctest/doctest.h"
#include "sym2/constants.h"
#include "sym2/expr.h"
#include "testutils.h"

using namespace sym2;

TEST_CASE("Equality")
{
    std::pmr::memory_resource* mr = std::pmr::get_default_resource();
    const Expr s = directSum(mr, 42_ex, "a"_ex, "b"_ex);
    const Expr pr = directProduct(mr, 42_ex, "a"_ex, "b"_ex);
    const Expr pw = directPower(mr, 42_ex, "a"_ex);

    SUBCASE("Identity equality")
    {
        const FixedExpr<1> n = 42_ex;
        const FixedExpr<1> a = "a"_ex;

        for (ExprView<> e : std::array<ExprView<>, 6>{n, a, pi, s, pr, pw})
            CHECK(e == e);
    }

    SUBCASE("Scalar Equality by value")
    {
        CHECK_EQ(Expr{"a", mr}, "a"_ex);
        CHECK_EQ(Expr{42, mr}, 42_ex);
        CHECK_EQ(Expr{0, mr}, 0_ex);
        CHECK_EQ(Expr{1, mr}, 1_ex);
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
        CHECK_NE(Expr{"a", mr}, "b"_ex);
        CHECK_NE(Expr{42, mr}, 43_ex);
    }
}
