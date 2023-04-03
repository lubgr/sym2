
#include <cerrno>
#include <cfenv>
#include <cmath>
#include <stdexcept>
#include <string_view>
#include <unordered_map>
#include "doctest/doctest.h"
#include "sym2/autosimpl.h"
#include "sym2/constants.h"
#include "sym2/eval.h"
#include "sym2/expr.h"
#include "testutils.h"

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
    const Expr::allocator_type alloc{};

    SUBCASE("Integer")
    {
        CHECK(evalReal(42_ex, lookupThrow) == doctest::Approx(42.0));
    }

    SUBCASE("Double")
    {
        CHECK(evalReal(Expr{1.23456789, alloc}, lookupThrow) == doctest::Approx(1.23456789));
    }

    SUBCASE("Small rational")
    {
        CHECK(evalReal(Expr{2, 3, alloc}, lookupThrow) == doctest::Approx(2.0 / 3.0));
    }

    SUBCASE("Constant")
    {
        CHECK(evalReal(pi, lookupThrow) == doctest::Approx(M_PI));
    }

    SUBCASE("Complex")
    {
        const Expr cplx = directComplex(2_ex, 3_ex, alloc);

        CHECK(evalComplex(cplx, lookupThrow) == std::complex<double>{2, 3});
        CHECK(evalReal(cplx, lookupThrow) == doctest::Approx(2.0));
    }
}

TEST_CASE("Numeric evaluation of composites without lookup")
{
    const Expr::allocator_type alloc{};
    const Expr sqrtTwo = directPower(2_ex, Expr{1, 2, alloc}, alloc);
    const Expr sqrtThree = directPower(3_ex, Expr{1, 2, alloc}, alloc);

    SUBCASE("Sqrt(2) + e")
    {
        const Expr what = directSum({sqrtTwo, euler}, alloc);
        const double expected = std::sqrt(2.0) + M_E;

        CHECK(evalCheckImagZero(what) == doctest::Approx(expected));
    }

    SUBCASE("2*sqrt(3)")
    {
        const Expr what = directProduct({2_ex, sqrtThree}, alloc);
        const double expected = 2.0 * std::sqrt(3.0);

        CHECK(evalCheckImagZero(what) == doctest::Approx(expected));
    }

    SUBCASE("1 + Pi + sqrt(2) + sqrt(3)*4^(1/17)")
    {
        const double expected =
          1.0 + M_PI + std::sqrt(2.0) + std::sqrt(3.0) * std::pow(4.0, 1.0 / 17.0);
        const Expr p =
          directProduct({sqrtThree, directPower(4_ex, Expr{1, 17, alloc}, alloc)}, alloc);
        const Expr what = directSum({1_ex, pi, sqrtTwo, p}, alloc);

        CHECK(evalCheckImagZero(what) == doctest::Approx(expected));
    }

    SUBCASE("-2*sqrt(3)*4^(1/3)*Pi")
    {
        const double expected = -2.0 * std::sqrt(3.0) * std::pow(4.0, 1.0 / 3.0) * M_PI;
        const Expr what = directProduct(
          {FixedExpr<1>{-2}, sqrtThree, directPower(4_ex, Expr{1, 3, alloc}, alloc), pi}, alloc);

        CHECK(evalCheckImagZero(what) == doctest::Approx(expected));
    }

    SUBCASE("atan2(42, 43)")
    {
        const double expected = std::atan2(42.0, 43.0);
        const Expr what{"atan2", 42_ex, 43_ex, std::atan2, alloc};

        CHECK(evalCheckImagZero(what) == doctest::Approx(expected));
    }
}

TEST_CASE("Numeric evaluation with lookup")
{
    const Expr::allocator_type alloc{};

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

        CHECK(evalCheckImagZero(Expr{"abcdefghijkl", alloc}, lookup) == doctest::Approx(42.0));
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
        const auto what = directSum(
          {"a"_ex, directProduct({2_ex, "b"_ex, directPower("c"_ex, "d"_ex, alloc)}, alloc),
            directProduct({FixedExpr<1>{-1}, directPower(3_ex, Expr{1, 2, alloc}, alloc),
                            Expr{"tan", "a"_ex, std::tan, alloc}},
              alloc)},
          alloc);

        CHECK(evalCheckImagZero(what, fp) == doctest::Approx(expected));
    }
}

TEST_CASE("Numeric evaluation to complex")
{
    const Expr::allocator_type alloc{};

    SUBCASE("Complex number")
    {
        const Expr cplx = directComplex(2_ex, 3_ex, alloc);
        const std::complex<double> actual = evalComplex(cplx, lookupThrow);

        CHECK(actual.real() == doctest::Approx(2.0));
        CHECK(actual.imag() == doctest::Approx(3.0));
    }

    SUBCASE("Real number")
    {
        const std::complex<double> actual = evalComplex(Expr{1.2345, alloc}, lookupThrow);

        CHECK(actual.real() == doctest::Approx(1.2345));
        CHECK(actual.imag() == doctest::Approx(0.0));
    }
}

TEST_CASE("Numeric power evaluation with complex/real distinction")
{
    const Expr::allocator_type alloc{};
    const double base = -42.0;
    const double exp = 9.87654;
    const Expr what = directPower(FixedExpr<2>{base}, FixedExpr<2>{exp}, alloc);
    const std::complex<double> expected = std::pow(std::complex<double>{base}, exp);

    SUBCASE("Complex exponentiation")
    {
        CHECK(evalComplex(what, lookupThrow).real() == doctest::Approx(expected.real()));
        CHECK(evalComplex(what, lookupThrow).imag() == doctest::Approx(expected.imag()));
    }
}
