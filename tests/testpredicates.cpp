
#include "autosimpl.h"
#include "constants.h"
#include "doctest/doctest.h"
#include "predicates.h"
#include "query.h"
#include "trigonometric.h"

using namespace sym2;

TEST_CASE("Type queries for untagged types")
{
    const Expr fp{3.14};
    const Expr sr{7, 11};
    const LargeInt largeInt{"2323498273984729837498234029380492839489234902384"};
    const Expr li{LargeIntRef{largeInt}};
    const Expr lr{LargeRationalRef{LargeRational{LargeInt{"1234528973498279834827384284"}, largeInt}}};
    const Expr cx = autoComplex(2_ex, 3_ex);
    const Expr s = autoSum(42_ex, "a"_ex, "b"_ex);
    const Expr pr = autoProduct(42_ex, "a"_ex, "b"_ex);
    const Expr pw = autoPower(42_ex, "a"_ex);
    const Expr sinA = sin("a"_ex);
    const Expr atan2Ab = atan2("a"_ex, "b"_ex);

    SUBCASE("Numeric subtypes")
    {
        CHECK(is<integer>(li));
        CHECK(is < large && integer > (li));

        CHECK(is<rational>(lr));
        CHECK(is < large && rational > (lr));

        CHECK(is<integer>(42_ex));
        CHECK(is < small && integer > (42_ex));

        CHECK(is < small && rational > (sr));

        CHECK(is<floatingPoint>(fp));

        for (ExprView<> n : {fp, sr, li, lr})
            CHECK(is < realDomain && number > (n));

        CHECK(is < complexDomain && number > (cx));
    }

    SUBCASE("Small/large distinction")
    {
        for (ExprView<> e : {42_ex, "a"_ex, pi, fp, sr}) {
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
        for (ExprView<> e : {42_ex, cx, "d"_ex, "a"_ex, "b"_ex, euler, pi})
            CHECK(is<scalar>(e));

        for (ExprView<> e : {pw, pr, s, sinA, atan2Ab})
            CHECK_FALSE(is<scalar>(e));
    }

    SUBCASE("Number")
    {
        for (ExprView<> n : {42_ex, fp, sr, li, lr, cx})
            CHECK(is<number>(n));

        for (ExprView<> e : {pi, euler, "a"_ex, s, pr, pw, sinA, atan2Ab})
            CHECK_FALSE(is<number>(e));
    }

    SUBCASE("Integer")
    {
        for (ExprView<> number : {42_ex, Expr{li}})
            CHECK(is<integer>(number));

        for (ExprView<> e : {pi, euler, "a"_ex, s, pr, pw, fp, sr, lr, cx, sinA, atan2Ab})
            CHECK_FALSE(is<integer>(e));
    }

    SUBCASE("Constant")
    {
        CHECK(is<constant>(pi));
        CHECK(is<constant>(euler));

        for (ExprView<> e : {42_ex, fp, sr, lr, cx, "a"_ex, s, pr, pw, sinA, atan2Ab})
            CHECK_FALSE(is<constant>(e));
    }

    SUBCASE("Symbol")
    {
        for (ExprView<> e : {"a"_ex, "b"_ex, "c"_ex})
            CHECK(is<symbol>(e));

        for (ExprView<> e : {42_ex, fp, sr, lr, cx, pi, euler, s, pr, pw, sinA, atan2Ab})
            CHECK_FALSE(is<symbol>(e));
    }

    SUBCASE("Named types")
    {
        for (ExprView<> e : {"a"_ex, pi, euler, atan2Ab, sinA})
            CHECK(is < symbol || function || constant > (e));

        for (ExprView<> e : {42_ex, fp, sr, lr, cx, s, pr, pw, sinA, atan2Ab})
            CHECK_FALSE(is < symbol || constant > (e));
    }

    SUBCASE("Sum")
    {
        CHECK(is<sum>(s));

        for (ExprView<> e : {42_ex, "a"_ex, pi, fp, sr, lr, cx, pr, pw, sinA, atan2Ab})
            CHECK_FALSE(is<sum>(e));
    }

    SUBCASE("Product")
    {
        CHECK(is<product>(pr));

        for (ExprView<> e : {42_ex, "a"_ex, pi, fp, sr, lr, cx, s, pw, sinA, atan2Ab})
            CHECK_FALSE(is<product>(e));
    }

    SUBCASE("Power")
    {
        CHECK(is<power>(pw));

        for (ExprView<> e : {42_ex, "a"_ex, pi, fp, sr, lr, cx, s, pr, sinA, atan2Ab})
            CHECK_FALSE(is<power>(e));
    }

    SUBCASE("Function")
    {
        CHECK(is<function>(sinA));
        CHECK(is<function>(atan2Ab));

        for (ExprView<> e : {42_ex, "a"_ex, pi, fp, sr, lr, cx, s, pr}) {
            CHECK_FALSE(is<function>(e));
        }
    }
}

TEST_CASE("Type queries for tagged types")
{
    const Expr n = 42_ex;

    CHECK(is<number>(tag < number && positive && realDomain > (n)));
    CHECK(is<small>(tag<number>(n)));
    CHECK(is < small && realDomain > (tag<number>(n)));
    CHECK(is < small && number && realDomain > (tag<number>(n)));
    CHECK(is<number>(tag < !symbol && !function && !(sum || power || complexDomain || !small) > (n)));
}
