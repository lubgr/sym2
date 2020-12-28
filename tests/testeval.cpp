
#include <cmath>
#include <stdexcept>
#include <string_view>
#include "canonical.h"
#include "constants.h"
#include "doctest/doctest.h"
#include "eval.h"

using namespace sym2;

namespace {
    const auto lookupThrow = [](std::string_view) -> double { throw std::domain_error{""}; };

    template <class LookupFct = decltype(lookupThrow)>
    double evalCheckImagZero(ExprView e, LookupFct lookup = lookupThrow)
    {
        const std::complex<double> result = evalComplex(e, lookup);

        CHECK(result.imag() == doctest::Approx(0.0));

        return result.real();
    }
}

TEST_CASE("Numeric evaluation of numerics/constant")
{
    SUBCASE("Constant")
    {
        CHECK(eval(pi, lookupThrow) == doctest::Approx(M_PI));
    }

    SUBCASE("Complex")
    {
        const Expr cplx{Type::complexNumber, {2_ex, 3_ex}};

        CHECK(evalComplex(cplx, lookupThrow) == std::complex<double>{2, 3});
        CHECK(eval(cplx, lookupThrow) == doctest::Approx(2.0));
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

    SUBCASE("1 + Pi + sqrt(2) + sqrt(3)*4^(1/17)")
    {
        const double expected = 1.0 + M_PI + std::sqrt(2.0) + std::sqrt(3.0) * std::pow(4.0, 1.0 / 17.0);
        const Expr p = product(sqrtThree, power(4, Expr{1, 17}));
        const Expr what = sum(1, pi, sqrtTwo, p);

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
}
