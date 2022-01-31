
#include <cerrno>
#include <cfenv>
#include <cmath>
#include <stdexcept>
#include <string_view>
#include <unordered_map>
#include "autosimpl.h"
#include "constants.h"
#include "doctest/doctest.h"
#include "eval.h"
#include "exprliteral.h"
#include "trigonometric.h"

using namespace sym2;

namespace {
    const auto lookupThrow = [](std::string_view) -> double { throw std::domain_error{""}; };

    template <class LookupFct = decltype(lookupThrow)>
    double evalCheckImagZero(ExprView<> e, LookupFct lookup = lookupThrow)
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
    SUBCASE("Integer")
    {
        CHECK(evalReal(42_ex, lookupThrow) == doctest::Approx(42.0));
    }

    SUBCASE("Double")
    {
        CHECK(evalReal(1.23456789_ex, lookupThrow) == doctest::Approx(1.23456789));
    }

    SUBCASE("Small rational")
    {
        CHECK(evalReal(Expr{2, 3}, lookupThrow) == doctest::Approx(2.0 / 3.0));
    }

    SUBCASE("Constant")
    {
        CHECK(evalReal(pi, lookupThrow) == doctest::Approx(M_PI));
    }

    SUBCASE("Complex")
    {
        const Expr cplx{CompositeType::complexNumber, {2_ex, 3_ex}};

        CHECK(evalComplex(cplx, lookupThrow) == std::complex<double>{2, 3});
        CHECK(evalReal(cplx, lookupThrow) == doctest::Approx(2.0));
    }
}

TEST_CASE("Numeric evaluation of composites without lookup")
{
    const Expr sqrtTwo = autoPower(2_ex, Expr{1, 2});
    const Expr sqrtThree = autoPower(3_ex, Expr{1, 2});

    SUBCASE("Sqrt(2) + e")
    {
        const Expr what = autoSum(sqrtTwo, euler);
        const double expected = std::sqrt(2.0) + M_E;

        CHECK(evalCheckImagZero(what) == doctest::Approx(expected));
    }

    SUBCASE("2*sqrt(3)")
    {
        const Expr what = autoProduct(2_ex, sqrtThree);
        const double expected = 2.0 * std::sqrt(3.0);

        CHECK(evalCheckImagZero(what) == doctest::Approx(expected));
    }

    SUBCASE("1 + Pi + sqrt(2) + sqrt(3)*4^(1/17)")
    {
        const double expected = 1.0 + M_PI + std::sqrt(2.0) + std::sqrt(3.0) * std::pow(4.0, 1.0 / 17.0);
        const Expr p = autoProduct(sqrtThree, autoPower(4_ex, Expr{1, 17}));
        const Expr what = autoSum(1_ex, pi, sqrtTwo, p);

        CHECK(evalCheckImagZero(what) == doctest::Approx(expected));
    }

    SUBCASE("-2*sqrt(3)*4^(1/3)*Pi")
    {
        const double expected = -2.0 * std::sqrt(3.0) * std::pow(4.0, 1.0 / 3.0) * M_PI;
        const Expr what = autoProduct(Expr{-2}, sqrtThree, autoPower(4_ex, Expr{1, 3}), pi);

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
        const auto what = autoSum("a"_ex, autoProduct(2_ex, "b"_ex, autoPower("c"_ex, "d"_ex)),
          autoProduct(Expr{-1}, autoPower(3_ex, Expr{1, 2}), tan("e"_ex)));

        CHECK(evalCheckImagZero(what, fp) == doctest::Approx(expected));
    }
}

TEST_CASE("Numeric evaluation to complex")
{
    SUBCASE("Complex number")
    {
        const Expr cplx{CompositeType::complexNumber, {2_ex, 3_ex}};
        const std::complex<double> actual = evalComplex(cplx, lookupThrow);

        CHECK(actual.real() == doctest::Approx(2.0));
        CHECK(actual.imag() == doctest::Approx(3.0));
    }

    SUBCASE("Real number")
    {
        const std::complex<double> actual = evalComplex(1.2345_ex, lookupThrow);

        CHECK(actual.real() == doctest::Approx(1.2345));
        CHECK(actual.imag() == doctest::Approx(0.0));
    }
}

TEST_CASE("Numeric power evaluation with complex/real distinction")
{
    const double base = -42.0;
    const double exp = 9.87654;
    const auto what = autoPower(Expr{base}, Expr{exp});
    const auto expected = std::pow(std::complex<double>{base}, exp);

    SUBCASE("Complex exponentiation")
    {
        CHECK(evalComplex(what, lookupThrow).real() == doctest::Approx(expected.real()));
        CHECK(evalComplex(what, lookupThrow).imag() == doctest::Approx(expected.imag()));
    }

    SUBCASE("Real exponentiation yields real part")
    {
        const auto expectedReal = std::real(expected);

        CHECK(evalReal(what, lookupThrow) == doctest::Approx(expectedReal));
    }
}
