
#include <cerrno>
#include <cfenv>
#include <cmath>
#include <stdexcept>
#include <string_view>
#include <unordered_map>
#include "canonical.h"
#include "constants.h"
#include "doctest/doctest.h"
#include "eval.h"
#include "trigonometric.h"

using namespace sym2;

namespace {
    const auto lookupThrow = [](std::string_view) -> double { throw std::domain_error{""}; };

    template <class LookupFct = decltype(lookupThrow)>
    double evalCheckImagZero(ExprView e, LookupFct lookup = lookupThrow)
    {
        const std::complex<double> result = evalComplex(e, lookup);
        const double realOnly = evalReal(e, lookup);

        CHECK(result.imag() == doctest::Approx(0.0));
        CHECK(result.real() == doctest::Approx(realOnly));

        return realOnly;
    }
}

TEST_CASE("Numeric evaluation of numerics/constant")
{
    SUBCASE("Constant")
    {
        CHECK(evalReal(pi, lookupThrow) == doctest::Approx(M_PI));
    }

    SUBCASE("Complex")
    {
        const Expr cplx{Type::complexNumber, {2_ex, 3_ex}};

        CHECK(evalComplex(cplx, lookupThrow) == std::complex<double>{2, 3});
        CHECK(evalReal(cplx, lookupThrow) == doctest::Approx(2.0));
    }
}

TEST_CASE("Numeric evaluation of composites without lookup")
{
    const Expr sqrtTwo = power(2, Expr{1, 2});
    const Expr sqrtThree = power(3, Expr{1, 2});

    SUBCASE("Sqrt(2) + e")
    {
        const Expr what = sum(sqrtTwo, euler);
        const double expected = std::sqrt(2.0) + M_E;

        CHECK(evalCheckImagZero(what) == doctest::Approx(expected));
    }

    SUBCASE("2*sqrt(3)")
    {
        const Expr what = product(2, sqrtThree);
        const double expected = 2.0 * std::sqrt(3.0);

        CHECK(evalCheckImagZero(what) == doctest::Approx(expected));
    }

    SUBCASE("1 + Pi + sqrt(2) + sqrt(3)*4^(1/17)")
    {
        const double expected = 1.0 + M_PI + std::sqrt(2.0) + std::sqrt(3.0) * std::pow(4.0, 1.0 / 17.0);
        const Expr p = product(sqrtThree, power(4, Expr{1, 17}));
        const Expr what = sum(1, pi, sqrtTwo, p);

        CHECK(evalCheckImagZero(what) == doctest::Approx(expected));
    }

    SUBCASE("-2*sqrt(3)*4^(1/3)*Pi")
    {
        const double expected = -2.0 * std::sqrt(3.0) * std::pow(4.0, 1.0 / 3.0) * M_PI;
        const Expr what = product(-2, sqrtThree, power(4, Expr{1, 3}), pi);

        CHECK(evalCheckImagZero(what) == doctest::Approx(expected));
    }

    SUBCASE("atan2(42, 43)")
    {
        const double expected = std::atan2(42.0, 43.0);
        const Expr what = sym2::atan2(42_ex, 43_ex);

        CHECK(evalCheckImagZero(what) == doctest::Approx(expected));
    }
}

TEST_CASE("Numeric evaluation with lookup")
{
    SUBCASE("Short symbol")
    {
        const auto lookup = [](std::string_view symbol) {
            CHECK(symbol == "a");
            return 42.0;
        };

        CHECK(evalCheckImagZero("a"_ex, lookup) == doctest::Approx(42.0));
    }

    SUBCASE("Long symbol")
    {
        const auto lookup = [](std::string_view symbol) {
            CHECK(symbol == "abcdefghijkl");
            return 42.0;
        };

        CHECK(evalCheckImagZero("abcdefghijkl"_ex, lookup) == doctest::Approx(42.0));
    }

    SUBCASE("a + 2*b*c^d - sqrt(3)*tan(e)")
    {
        static const std::unordered_map<std::string_view, double> table{
          {"a", 123.456},
          {"b", 456.789},
          {"c", 42.0},
          {"d", 9.876},
          {"e", 2.345},
        };
        const auto fp = [](std::string_view symbol) { return table.at(symbol); };
        const double expected =
          fp("a") + 2.0 * fp("b") * std::pow(fp("c"), fp("d")) - std::sqrt(3.0) * std::tan(fp("e"));
        const auto what = sum("a", product(2, "b", power("c", "d")), product(-1, power(3, Expr{1, 2}), tan("e"_ex)));

        CHECK(evalCheckImagZero(what, fp) == doctest::Approx(expected));
    }
}

TEST_CASE("Numeric evaluation with complex/real distinction")
{
    const double base = -42.0;
    const double exp = 9.87654;
    const auto what = power(base, exp);

    SUBCASE("Complex exponentiation is solvable")
    {
        const auto expected = std::pow(std::complex<double>{base}, exp);

        CHECK(evalComplex(what, lookupThrow).real() == doctest::Approx(expected.real()));
        CHECK(evalComplex(what, lookupThrow).imag() == doctest::Approx(expected.imag()));
    }

    SUBCASE("Real exponentiation yields NaN")
    {
        std::feclearexcept(FE_ALL_EXCEPT);

        const double probablyNan = evalReal(what, lookupThrow);

        CHECK(std::fetestexcept(FE_INVALID));

        (void) probablyNan;
    }
}
