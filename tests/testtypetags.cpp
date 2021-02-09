
#include "autosimpl.h"
#include "constants.h"
#include "doctest/doctest.h"
#include "query.h"
#include "trigonometric.h"
#include "typetags.h"

using namespace sym2;

TEST_CASE("Type queries for untagged types")
{
    const Expr fp{3.14};
    const Expr sr{7, 11};
    const LargeInt largeInt{"2323498273984729837498234029380492839489234902384"};
    const Expr li{largeInt};
    const Expr lr{LargeRational{LargeInt{"1234528973498279834827384284"}, largeInt}};
    const Expr cx = cpx(2_ex, 3_ex);
    const Expr s = sum(42_ex, "a"_ex, "b"_ex);
    const Expr pr = product(42_ex, "a"_ex, "b"_ex);
    const Expr pw = power(42_ex, "a"_ex);
    const Expr sinA = sin("a"_ex);
    const Expr atan2Ab = atan2("a"_ex, "b"_ex);

    SUBCASE("Numeric subtypes")
    {
        CHECK(is<Int>(li));
        CHECK(is<Large, Int>(li));

        CHECK(is<Rational>(lr));
        CHECK(is<Large, Rational>(lr));

        CHECK(is<Int>(42_ex));
        CHECK(is<Small, Int>(42_ex));

        CHECK(is<Small, Rational>(sr));

        CHECK(is<Double>(fp));

        for (ExprView n : {fp, sr, li, lr})
            CHECK(is<Real, Number>(n));

        CHECK(is<Complex, Number>(cx));
    }

    SUBCASE("Small/Large distinction")
    {
        for (ExprView e : {42_ex, "a"_ex, pi, fp, sr}) {
            CHECK(is<Small>(e));
            CHECK(!is<Large>(e));
        }

        for (ExprView e : {pr, lr, cx, s, pw, sinA, atan2Ab}) {
            CHECK(is<Large>(e));
            CHECK(!is<Small>(e));
        }
    }

    SUBCASE("Scalar")
    {
        for (ExprView e : {42_ex, cx, "d"_ex, "a"_ex, "b"_ex, euler, pi})
            CHECK(is<Scalar>(e));

        for (ExprView e : {pw, pr, s, sinA, atan2Ab})
            CHECK_FALSE(is<Scalar>(e));
    }

    SUBCASE("Number")
    {
        for (ExprView number : {42_ex, fp, sr, li, lr, cx})
            CHECK(is<Number>(number));

        for (ExprView e : {pi, euler, "a"_ex, s, pr, pw, sinA, atan2Ab})
            CHECK_FALSE(is<Number>(e));
    }

    SUBCASE("Integer")
    {
        for (ExprView number : {42_ex, Expr{li}})
            CHECK(is<Int>(number));

        for (ExprView e : {pi, euler, "a"_ex, s, pr, pw, fp, sr, lr, cx, sinA, atan2Ab})
            CHECK_FALSE(is<Int>(e));
    }

    SUBCASE("Constant")
    {
        CHECK(is<Constant>(pi));
        CHECK(is<Constant>(euler));

        for (ExprView e : {42_ex, fp, sr, lr, cx, "a"_ex, s, pr, pw, sinA, atan2Ab})
            CHECK_FALSE(is<Constant>(e));
    }

    SUBCASE("Symbol")
    {
        for (ExprView e : {"a"_ex, "b"_ex, "c"_ex})
            CHECK(is<Symbol>(e));

        for (ExprView e : {42_ex, fp, sr, lr, cx, pi, euler, s, pr, pw, sinA, atan2Ab})
            CHECK_FALSE(is<Symbol>(e));
    }

    SUBCASE("Named types")
    {
        for (ExprView e : {"a"_ex, pi, euler, atan2Ab, sinA})
            CHECK(isOneOf<Symbol, Function, Constant>(e));

        for (ExprView e : {42_ex, fp, sr, lr, cx, s, pr, pw, sinA, atan2Ab})
            CHECK_FALSE(isOneOf<Symbol, Constant>(e));
    }

    SUBCASE("Sum")
    {
        CHECK(is<Sum>(s));

        for (ExprView e : {42_ex, "a"_ex, pi, fp, sr, lr, cx, pr, pw, sinA, atan2Ab})
            CHECK_FALSE(is<Sum>(e));
    }

    SUBCASE("Product")
    {
        CHECK(is<Product>(pr));

        for (ExprView e : {42_ex, "a"_ex, pi, fp, sr, lr, cx, s, pw, sinA, atan2Ab})
            CHECK_FALSE(is<Product>(e));
    }

    SUBCASE("Power")
    {
        CHECK(is<Power>(pw));

        for (ExprView e : {42_ex, "a"_ex, pi, fp, sr, lr, cx, s, pr, sinA, atan2Ab})
            CHECK_FALSE(is<Power>(e));
    }

    SUBCASE("Function")
    {
        CHECK(is<Function>(sinA));
        CHECK(is<Function>(atan2Ab));

        for (ExprView e : {42_ex, "a"_ex, pi, fp, sr, lr, cx, s, pr}) {
            CHECK_FALSE(is<Function>(e));
        }
    }
}

TEST_CASE("Type queries for tagged types")
{
    const Expr n = 42_ex;

    CHECK(is<Number>(tag<Number, Positive, Real>(n)));
    CHECK(is<Small>(tag<Number>(n)));
    CHECK(is<Small, Real>(tag<Number>(n)));
    CHECK(is<Small, Number, Real>(tag<Number>(n)));

    /* The tags don't need to be meaningful for a successful check: */
    CHECK(is<Number>(tag<Symbol, Function>(n)));
}
