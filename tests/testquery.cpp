
#include "sym2/autosimpl.h"
#include "sym2/constants.h"
#include "doctest/doctest.h"
#include "sym2/expr.h"
#include "sym2/predicates.h"
#include "sym2/query.h"
#include "testutils.h"
#include "trigonometric.h"

using namespace sym2;

TEST_CASE("Counters")
{
    auto* mr = std::pmr::get_default_resource();
    const Expr fp{3.14, mr};
    const Expr sr{7, 11, mr};
    const LargeInt largeInt{"2323498273984729837498234029380492839489234902384"};
    const Expr li{largeInt, mr};
    const Expr lr{LargeRational{LargeInt{"1234528973498279834827384284"}, largeInt}, mr};
    const Expr cx = directComplex(mr, 2_ex, 3_ex);
    const Expr s = directSum(mr, 42_ex, "a"_ex, "b"_ex);
    const Expr pr = directProduct(mr, 42_ex, "a"_ex, "b"_ex);
    const Expr pw = directPower(mr, 42_ex, "a"_ex);
    const Expr sinA{"sin", "a"_ex, std::sin, mr};
    const Expr atan2Ab{"atan2", "a"_ex, "b"_ex, std::atan2, mr};

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
    auto* mr = std::pmr::get_default_resource();

    SUBCASE("Sum with product")
    {
        const Expr pr = directProduct(mr, 10, "b", "c");
        const Expr s = directSum(mr, 42, "a", pr, "d");

        CHECK(firstOperand(s) == 42_ex);
        CHECK(secondOperand(s) == "a"_ex);
        CHECK(nthOperand(s, 2) == pr);
        CHECK(nthOperand(s, 3) == "d"_ex);
    }

    SUBCASE("Complex number")
    {
        const Expr cx = directComplex(mr, 2, 3);

        CHECK(real(cx) == 2_ex);
        CHECK(imag(cx) == 3_ex);

        const LargeInt realPartInt{"1298374982734923434528973498279834827384284"};
        const LargeRational imagPartRational{
          "87234728489237/2938749283749823423423468923428429238649826482"};
        const Expr realPart{realPartInt, mr};
        const Expr imagPart{imagPartRational, mr};
        const Expr largeCx = directComplex(mr, realPartInt, imagPartRational);

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
    auto* const mr = std::pmr::get_default_resource();
    const Expr pw = directPower(mr, "a", "b");

    SUBCASE("Power bursts into base and exponent")
    {
        const auto [base, exp] = splitAsPower(pw);

        CHECK(base == "a"_ex);
        CHECK(exp == "b"_ex);
    }
}
