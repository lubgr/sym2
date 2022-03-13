
#include "polynomial.h"
#include <cassert>
#include <limits>
#include <numeric>
#include <stdexcept>
#include "get.h"
#include "query.h"

std::int32_t sym2::polyMinDegreeNoValidityCheck(const ExprView<> of, ExprView<symbol> variable)
{
    const auto recurse = [variable](const ExprView<> of) {
        return polyMinDegreeNoValidityCheck(of, variable);
    };

    if (is<number>(of))
        return 0;
    else if (of == variable)
        return 1;
    else if (is<power>(of)) {
        const auto [base, exp] = splitAsPower(of);

        assert(is < integer && small && positive > (exp));

        return get<std::int32_t>(exp) * recurse(base);
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

std::int32_t sym2::polyMinDegreeWithValidityCheck(const ExprView<> of, ExprView<symbol> variable)
{
    if (!isValidPolynomial(of))
        throw std::domain_error{"Min. polynomial degree query for invalid polynomail input"};

    return polyMinDegreeNoValidityCheck(of, variable);
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
