
#include <array>
#include "sym2/constants.h"
#include "doctest/doctest.h"
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
        CHECK(Expr{"a", mr} == "a"_ex);
        CHECK(Expr{42, mr} == 42_ex);
        CHECK(Expr{0, mr} == 0_ex);
        CHECK(Expr{1, mr} == 1_ex);
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
        CHECK(Expr{"a", mr} != "b"_ex);
        CHECK(Expr{42, mr} != 43_ex);
    }
}
