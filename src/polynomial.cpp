
#include "sym2/polynomial.h"
#include <cassert>
#include <limits>
#include <numeric>
#include <stdexcept>
#include "sym2/expr.h"
#include "sym2/get.h"
#include "sym2/query.h"

std::int32_t sym2::degree(ExprView<> of, ExprView<> wrt)
{
    if (of == wrt)
        return 1;
    else if (is<number>(of))
        return 0;
    else if (is<power>(of)) {
        const auto [base, exp] = splitAsPower(of);

        if (is < integer && small > (exp)) {
            const std::int32_t baseDegree = degree(base, wrt);
            const auto expDegree = get<std::int16_t>(exp);

            // Check overflow using large integers, should only ever happen in completely
            // pathological cases.
            assert(get<LargeInt>(exp) * baseDegree <= std::numeric_limits<std::int32_t>::max());
            assert(get<LargeInt>(exp) * baseDegree >= std::numeric_limits<std::int32_t>::min());

            return baseDegree * expDegree;
        }

        return 0;
    }

    const auto recur = [wrt](const ExprView<> operand) { return degree(operand, wrt); };

    if (is<sum>(of)) {
        const OperandsView ops = OperandsView::operandsOf(of);
        const auto int32Max = [](std::int32_t lhs, std::int32_t rhs) { return std::max(lhs, rhs); };

        // We prefer transform_reduce over max_element to enforce a single call to degree per
        // operand. While both algorithms have linear time complexity, max_element invokes the
        // comparator N-1 times, which results in degree being called N-1 times more often than
        // with transform_reduce.
        return std::transform_reduce(
          ops.begin(), ops.end(), std::numeric_limits<std::int32_t>::min(), int32Max, recur);
    } else if (is<product>(of)) {
        const OperandsView ops = OperandsView::operandsOf(of);
        return std::transform_reduce(ops.begin(), ops.end(), std::int32_t{0}, std::plus<>{}, recur);
    }

    return 0;
}

std::int32_t sym2::minDegreeNoValidityCheck(const ExprView<> of, ExprView<symbol> variable)
{
    const auto recurse = [variable](
                           const ExprView<> of) { return minDegreeNoValidityCheck(of, variable); };

    if (is<number>(of))
        return 0;
    else if (of == variable)
        return 1;
    else if (is<power>(of)) {
        const auto [base, exp] = splitAsPower(of);

        assert(is < integer && small && positive > (exp));

        return get<std::int16_t>(exp) * recurse(base);
    } else if (is<sum>(of)) {
        auto result = std::numeric_limits<std::int32_t>::max();

        // The manual loop should be the fastest option here, since we avoid any space/allocation
        // overhead as well as multiple evaluations of the same recursive call. We had those
        // additional costs when transforming the summands into a container of integers first, or
        // when using a min_element algorithm (multiple evaluations without memoization, allocations
        // with memoization).
        for (const ExprView<> summand : OperandsView::operandsOf(of)) {
            const std::int32_t degree = recurse(summand);

            result = std::min(result, degree);
        }

        return result;
    } else if (is<product>(of)) {
        const OperandsView factors = OperandsView::operandsOf(of);

        return std::transform_reduce(factors.begin(), factors.end(), std::int32_t{0}, std::plus<>{},
          [recurse](const ExprView<!product> factor) { return recurse(factor); });
    }

    return 0;
}

std::int32_t sym2::minDegreeWithValidityCheck(const ExprView<> of, ExprView<symbol> variable)
{
    if (!isValidPolynomial(of))
        throw std::domain_error{"Min. polynomial degree query for invalid polynomail input"};

    return minDegreeNoValidityCheck(of, variable);
}

bool sym2::isValidPolynomial(const ExprView<> p)
{
    if (is<symbol>(p))
        return true;
    else if (is<rational>(p))
        return true;
    else if (is<power>(p)) {
        const auto [base, exp] = splitAsPower(p);

        return is < integer && small && positive > (exp) && isValidPolynomial(base);
    } else if (is < sum || product > (p)) {
        const OperandsView ops = OperandsView::operandsOf(p);

        return std::all_of(ops.begin(), ops.end(), &isValidPolynomial);
    }

    return false;
}

sym2::ExprView<> sym2::coefficient(ExprView<> of, ExprView<> wrt, std::int32_t exponent)
{
    static const auto zero = 0_ex;
    static const auto one = 1_ex;

    if (of == wrt)
        return exponent == 1 ? one : zero;
    else if (!contains(of, wrt) && exponent == 0)
        return of;
    else
        return zero;
}

sym2::ExprView<> sym2::leadingCoefficient(ExprView<> of, ExprView<> wrt)
{
    return coefficient(of, wrt, degree(of, wrt));
}
