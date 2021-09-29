
#include "autosimpl.h"
#include "constants.h"
#include "doctest/doctest.h"
#include "predicates.h"
#include "query.h"
#include "trigonometric.h"

using namespace sym2;

TEST_CASE("Counters")
{
    const Expr fp{3.14};
    const Expr sr{7, 11};
    const LargeInt largeInt{"2323498273984729837498234029380492839489234902384"};
    const Expr li{largeInt};
    const Expr lr{LargeRational{LargeInt{"1234528973498279834827384284"}, largeInt}};
    const Expr cx = autoComplex(2_ex, 3_ex);
    const Expr s = autoSum(42_ex, "a"_ex, "b"_ex);
    const Expr pr = autoProduct(42_ex, "a"_ex, "b"_ex);
    const Expr pw = autoPower(42_ex, "a"_ex);
    const Expr sinA = sin("a"_ex);
    const Expr atan2Ab = atan2("a"_ex, "b"_ex);

    SUBCASE("Number of operands")
    {
        for (ExprView<> e : {"a"_ex, 42_ex, fp, sr, pi, euler, lr})
            CHECK(nOperands(e) == 0);

        CHECK(nOperands(cx) == 2);
        CHECK(nOperands(s) == 3);
        CHECK(nOperands(pr) == 3);
        CHECK(nOperands(pw) == 2);
        CHECK(nOperands(sinA) == 1);
        CHECK(nOperands(atan2Ab) == 2);
    }

    SUBCASE("Number of child blobs")
    {
        const auto composite = autoSum(sinA, autoProduct(pw, s), pr, cx, lr, atan2Ab);

        for (ExprView<> e : {"a"_ex, 42_ex, fp, sr, pi, euler, li, lr, cx})
            CHECK(nPhysicalChildren(e) == e.size() - 1);
    }
}

TEST_CASE("Physical and logical children queries")
{
    SUBCASE("Sum with product")
    {
        const Expr pr = autoProduct(10_ex, "b"_ex, "c"_ex);
        const Expr s = autoSum(42_ex, "a"_ex, pr, "d"_ex);

        CHECK(firstOperand(s) == 42_ex);
        CHECK(secondOperand(s) == "a"_ex);
        CHECK(nthOperand(s, 3) == pr);
        CHECK(nthOperand(s, 4) == "d"_ex);
    }

    SUBCASE("Large Rational")
    {
        const LargeInt denom{"1234528973498279834827384284"};
        const Expr lr{LargeRational{1, denom}};

        CHECK(numerator(lr) == 1_ex);
        CHECK(denominator(lr) == Expr{denom});
    }

    SUBCASE("Complex number")
    {
        const Expr cx = autoComplex(2_ex, 3_ex);

        CHECK(real(cx) == 2_ex);
        CHECK(imag(cx) == 3_ex);
    }
}

TEST_CASE("Deconstruct as power")
{
    const auto a = "a"_ex;
    const auto b = "b"_ex;
    const Expr pw = autoPower(a, b);

    SUBCASE("Power bursts into base and exponent")
    {
        SUBCASE("Untagged")
        {
            const auto [base, exp] = asPower(pw);

            CHECK(base == a);
            CHECK(exp == b);
        }

        SUBCASE("Tagged")
        {
            const auto [base, exp] = asPower(tag<power>(pw));

            CHECK(base == a);
            CHECK(exp == b);
        }
    }
}
