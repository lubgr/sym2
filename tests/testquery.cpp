
#include "doctest/doctest.h"
#include "sym2/autosimpl.h"
#include "sym2/constants.h"
#include "sym2/expr.h"
#include "sym2/predicates.h"
#include "sym2/query.h"
#include "testutils.h"
#include "trigonometric.h"

using namespace sym2;

TEST_CASE("Counters")
{
    const Expr::allocator_type alloc{};
    const Expr fp{3.14, alloc};
    const Expr sr{7, 11, alloc};
    const LargeInt largeInt{"2323498273984729837498234029380492839489234902384"};
    const Expr li{largeInt, alloc};
    const Expr lr{LargeRational{LargeInt{"1234528973498279834827384284"}, largeInt}, alloc};
    const Expr cx = directComplex(2_ex, 3_ex, alloc);
    const Expr s = directSum({42_ex, "a"_ex, "b"_ex}, alloc);
    const Expr pr = directProduct({42_ex, "a"_ex, "b"_ex}, alloc);
    const Expr pw = directPower(42_ex, "a"_ex, alloc);
    const Expr sinA{"sin", "a"_ex, std::sin, alloc};
    const Expr atan2Ab{"atan2", "a"_ex, "b"_ex, std::atan2, alloc};

    SUBCASE("Number of operands")
    {
        CHECK(nOperands("a"_ex) == 0);
        CHECK(nOperands(42_ex) == 0);
        CHECK(nOperands(fp) == 0);
        CHECK(nOperands(sr) == 0);
        CHECK(nOperands(pi) == 0);
        CHECK(nOperands(lr) == 0);
        CHECK(nOperands(cx) == 0);

        CHECK(nOperands(s) == 3);
        CHECK(nOperands(pr) == 3);
        CHECK(nOperands(pw) == 2);
        CHECK(nOperands(sinA) == 1);
        CHECK(nOperands(atan2Ab) == 2);
    }
}

TEST_CASE("Logical children queries")
{
    const Expr::allocator_type alloc{};

    SUBCASE("Sum with product")
    {
        const Expr pr = directProduct({10_ex, "b"_ex, "c"_ex}, alloc);
        const Expr s = directSum({42_ex, "a"_ex, pr, "d"_ex}, alloc);

        CHECK(firstOperand(s) == 42_ex);
        CHECK(secondOperand(s) == "a"_ex);
        CHECK(nthOperand(s, 2) == pr);
        CHECK(nthOperand(s, 3) == "d"_ex);
    }

    SUBCASE("Complex number")
    {
        const Expr cx = directComplex(2_ex, 3_ex, alloc);

        CHECK(real(cx) == 2_ex);
        CHECK(imag(cx) == 3_ex);

        const LargeInt realPartInt{"1298374982734923434528973498279834827384284"};
        const LargeRational imagPartRational{
          "87234728489237/2938749283749823423423468923428429238649826482"};
        const Expr realPart{realPartInt, alloc};
        const Expr imagPart{imagPartRational, alloc};
        const Expr largeCx = directComplex(realPart, imagPart, alloc);

        CHECK(real(largeCx) == realPart);
        CHECK(imag(largeCx) == imagPart);
    }

    SUBCASE("Complex query for non-complex number")
    {
        CHECK(real(42_ex) == 42_ex);
        CHECK(imag(42_ex) == 0_ex);
    }
}

TEST_CASE("Deconstruct as power")
{
    const Expr::allocator_type alloc{};
    const Expr pw = directPower("a"_ex, "b"_ex, alloc);

    SUBCASE("Power bursts into base and exponent")
    {
        const auto [base, exp] = splitAsPower(pw);

        CHECK(base == "a"_ex);
        CHECK(exp == "b"_ex);
    }
}
