
#include "autosimpl.h"
#include "constants.h"
#include "doctest/doctest.h"
#include "exprliteral.h"
#include "predicates.h"
#include "query.h"
#include "trigonometric.h"

using namespace sym2;

TEST_CASE("Type queries for untagged types")
{
    std::pmr::memory_resource* mr = std::pmr::get_default_resource();
    const Expr fp{3.14, mr};
    const Expr sr{7, 11, mr};
    const Expr a{"a"_ex, mr};
    const Expr n{42_ex, mr};
    const LargeInt largeInt{"2323498273984729837498234029380492839489234902384", mr};
    const Expr li{LargeIntRef{largeInt}, mr};
    const Expr lr{
      LargeRationalRef{LargeRational{LargeInt{"1234528973498279834827384284"}, largeInt}}, mr};
    const Expr cx = autoComplex(2_ex, 3_ex);
    const Expr s = autoSum(n, a, "b"_ex);
    const Expr pr = autoProduct(n, a, "b"_ex);
    const Expr pw = autoPower(n, a);
    const Expr sinA = sin(a);
    const Expr atan2Ab = atan2(a, "b"_ex);

    SUBCASE("Numeric subtypes")
    {
        CHECK(is<integer>(li));
        CHECK(is < large && integer > (li));

        CHECK(is<rational>(lr));
        CHECK(is < large && rational > (lr));

        CHECK(is<integer>(n));
        CHECK(is < small && integer > (n));

        CHECK(is < small && rational > (sr));

        CHECK(is<floatingPoint>(fp));

        for (ExprView<> n : {fp, sr, li, lr})
            CHECK(is < realDomain && number > (n));

        CHECK(is < complexDomain && number > (cx));
    }

    SUBCASE("Small/large distinction")
    {
        for (ExprView<> e : {n, a, pi, fp, sr}) {
            CHECK(is<small>(e));
            CHECK(!is<large>(e));
        }

        for (ExprView<> e : {pr, lr, cx, s, pw, sinA, atan2Ab}) {
            CHECK(is<large>(e));
            CHECK(!is<small>(e));
        }
    }

    SUBCASE("Scalar")
    {
        for (ExprView<> e : {n, cx, Expr{"d", mr}, Expr{"a", mr}, Expr{"b", mr}, euler, pi}) {
            CHECK(is<scalar>(e));
            CHECK(is<!composite>(e));
        }

        for (ExprView<> e : {pw, pr, s, sinA, atan2Ab}) {
            CHECK_FALSE(is<scalar>(e));
            CHECK_FALSE(is<!composite>(e));
        }
    }

    SUBCASE("Number")
    {
        for (ExprView<> n : {n, fp, sr, li, lr, cx})
            CHECK(is<number>(n));

        for (ExprView<> e : {pi, euler, a, s, pr, pw, sinA, atan2Ab})
            CHECK_FALSE(is<number>(e));
    }

    SUBCASE("Integer")
    {
        for (ExprView<> number : {n, Expr{li, mr}})
            CHECK(is<integer>(number));

        for (ExprView<> e : {pi, euler, a, s, pr, pw, fp, sr, lr, cx, sinA, atan2Ab})
            CHECK_FALSE(is<integer>(e));
    }

    SUBCASE("Constant")
    {
        CHECK(is<constant>(pi));
        CHECK(is<constant>(euler));

        for (ExprView<> e : {n, fp, sr, lr, cx, a, s, pr, pw, sinA, atan2Ab})
            CHECK_FALSE(is<constant>(e));
    }

    SUBCASE("Symbol")
    {
        for (ExprView<> e : {Expr{"a", mr}, Expr{"b", mr}, Expr{"c", mr}})
            CHECK(is<symbol>(e));

        for (ExprView<> e : {n, fp, sr, lr, cx, pi, euler, s, pr, pw, sinA, atan2Ab})
            CHECK_FALSE(is<symbol>(e));
    }

    SUBCASE("Named types")
    {
        for (ExprView<> e : {a, pi, euler, atan2Ab, sinA})
            CHECK(is < symbol || function || constant > (e));

        for (ExprView<> e : {n, fp, sr, lr, cx, s, pr, pw, sinA, atan2Ab})
            CHECK_FALSE(is < symbol || constant > (e));
    }

    SUBCASE("Sum")
    {
        CHECK(is<sum>(s));

        for (ExprView<> e : {n, a, pi, fp, sr, lr, cx, pr, pw, sinA, atan2Ab})
            CHECK_FALSE(is<sum>(e));
    }

    SUBCASE("Product")
    {
        CHECK(is<product>(pr));

        for (ExprView<> e : {n, a, pi, fp, sr, lr, cx, s, pw, sinA, atan2Ab})
            CHECK_FALSE(is<product>(e));
    }

    SUBCASE("Power")
    {
        CHECK(is<power>(pw));

        for (ExprView<> e : {n, a, pi, fp, sr, lr, cx, s, pr, sinA, atan2Ab})
            CHECK_FALSE(is<power>(e));
    }

    SUBCASE("Function")
    {
        CHECK(is<function>(sinA));
        CHECK(is<function>(atan2Ab));

        for (ExprView<> e : {n, a, pi, fp, sr, lr, cx, s, pr}) {
            CHECK_FALSE(is<function>(e));
        }
    }
}

TEST_CASE("Type queries for tagged types")
{
    const ExprLiteral n = 42_ex;

    CHECK(is<number>(ExprView < number && positive && realDomain > {n}));
    CHECK(is<small>(ExprView<number>{n}));
    CHECK(is < small && realDomain > (ExprView<number>{n}));
    CHECK(is < small && number && realDomain > (ExprView<number>{n}));
    CHECK(is<number>(
      ExprView < !symbol && !function && !(sum || power || complexDomain || !small) > {n}));
}
