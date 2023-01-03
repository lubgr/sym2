
#include "sym2/predicates.h"
#include <cassert>
#include <queue>
#include "allocator.h"
#include "blobapi.h"
#include "operandsview.h"
#include "sym2/eval.h"
#include "sym2/expr.h"
#include "sym2/get.h"
#include "sym2/query.h"

namespace sym2 {
    enum class NonNumericSign { positive, negative, unknown, onlyNumeric };

    namespace {
        template <class ReductionOp>
        double reduceNumericallyEvaluable(ExprView<> e, ReductionOp reduce, const double init)
        {
            double result = init;

            for (const ExprView<> op : OperandsView::operandsOf(e)) {
                if (isNumericallyEvaluable(op))
                    result = reduce(result, get<double>(op));
            }

            return result;
        }

        NonNumericSign signOfNonNumericallyEvaluable(ExprView<sum> e)
        {
            bool foundPositive = false;
            bool foundNegative = false;

            for (const ExprView<> op : OperandsView::operandsOf(e)) {
                if (isNumericallyEvaluable(op))
                    continue;
                else if (isPositive(op))
                    foundPositive = true;
                else if (isNegative(op))
                    foundNegative = true;
                else
                    return NonNumericSign::unknown;

                if (foundPositive && foundNegative)
                    return NonNumericSign::unknown;
            }

            // Should have bailed out early
            assert(!(foundPositive && foundNegative));

            if (!foundPositive && !foundNegative)
                return NonNumericSign::onlyNumeric;

            assert(foundPositive || foundNegative);

            return foundPositive ? NonNumericSign::positive : NonNumericSign::negative;
        }

        short signOfProduct(ExprView<product> e)
        {
            short result = 1;

            for (const ExprView<!product> factor : OperandsView::operandsOf(e))
                if (isPositive(factor))
                    continue;
                else if (isNegative(factor))
                    result *= -1;
                else
                    return 0;

            return result;
        }

        template <std::size_t staticQueueSize, class Fct>
        bool bfs(ExprView<> root, Fct callback, const bool expected, const bool earlyReturn)
        {
            auto [_, mr] =
              monotonicStackPmrResource<ByteSize{staticQueueSize * sizeof(ExprView<>)}>();
            std::queue<ExprView<>, std::pmr::deque<ExprView<>>> lookAt{&mr};

            if (callback(root) != expected)
                return earlyReturn;

            lookAt.push(root);

            while (!lookAt.empty()) {
                for (const ExprView<> op : OperandsView::operandsOf(lookAt.front()))
                    if (callback(op) != expected)
                        return earlyReturn;
                    else
                        lookAt.push(op);

                lookAt.pop();
            }

            return !earlyReturn;
        }

        template <std::size_t staticQueueSize, class Fct>
        bool bfsAllOf(ExprView<> root, Fct callback)
        {
            return bfs<staticQueueSize>(root, callback, true, false);
        }

        template <std::size_t staticQueueSize, class Fct>
        bool bfsAnyOf(ExprView<> root, Fct callback)
        {
            return bfs<staticQueueSize>(root, callback, false, true);
        }

        template <std::size_t staticQueueSize, class Fct>
        bool bfsNoneOf(ExprView<> root, Fct callback)
        {
            return bfs<staticQueueSize>(root, callback, false, false);
        }
    }
}

bool sym2::isNumericallyEvaluable(ExprView<> e) noexcept
{
    return bfsNoneOf<64>(e, isSymbol);
}

bool sym2::isPositive(ExprView<> e) noexcept
{
    if (isSymbol(e))
        return getDomainFlag(e.get()) == DomainFlag::positive;
    else if (isNumericallyEvaluable(e)) {
        const std::complex<double> cx = evalComplex(e, [](auto&&...) {
            assert(false);
            return 0.0;
        });
        return std::abs(std::imag(cx)) < 1e-10 && std::real(cx) > 0.0;
    } else if (isSum(e)) {
        const NonNumericSign sign = signOfNonNumericallyEvaluable(e);

        if (sign == NonNumericSign::positive || sign == NonNumericSign::onlyNumeric)
            return reduceNumericallyEvaluable(e, std::plus<>{}, 0.0) >= 0.0;
    } else if (isProduct(e)) {
        return signOfProduct(e) == 1;
    } else if (isPower(e)) {
        const auto [base, exp] = splitAsPower(e);

        if (!is<realDomain>(base))
            return false;
        else if (is<positive>(base))
            return is<realDomain>(exp);
        else if (is < small && rational > (exp))
            return get<SmallRational>(exp).num % 2 == 0;
        else if (is < large && rational > (exp))
            return numerator(get<LargeRational>(exp)) % 2 == 0;
    }

    return false;
}

bool sym2::isNegative(ExprView<> e) noexcept
{
    if (isSymbol(e))
        return false;
    else if (isNumericallyEvaluable(e)) {
        const std::complex<double> cx = evalComplex(e, [](auto&&...) {
            assert(false);
            return 0.0;
        });
        return std::abs(std::imag(cx)) < 1e-10 && std::real(cx) < 0.0;
    } else if (isSum(e)) {
        const NonNumericSign sign = signOfNonNumericallyEvaluable(e);
        if (sign == NonNumericSign::onlyNumeric)
            return reduceNumericallyEvaluable(e, std::plus<>{}, 0.0) < 0.0;
        else if (sign == NonNumericSign::negative)
            return reduceNumericallyEvaluable(e, std::plus<>{}, 0.0) <= 0.0;
    } else if (isProduct(e))
        return signOfProduct(e) == -1;

    return false;
}

bool sym2::isRealDomain(ExprView<> e) noexcept
{
    return bfsAllOf<64>(e, [](ExprView<> op) {
        // TODO handle function domains
        if (isSymbolHeader(*op.get())) {
            const DomainFlag domain = getDomainFlag(op.get());
            return domain == DomainFlag::real || domain == DomainFlag::positive;
        } else if (isComplexNumberHeader(*op.get()))
            return false;
        else if (isNumberHeader(*op.get()))
            return true;
        else
            // We treat everything else, in particular composites, as real since the all-of
            // semantics will keep invoking this predicate for all leaves. Note that this also
            // treats constants as real, which might change in the future.
            return true;
    });
}

bool sym2::isComplexDomain(ExprView<> e) noexcept
{
    return bfsAllOf<64>(e, [](ExprView<> op) {
        // TODO handle function domains
        if (isSymbolHeader(*op.get())) {
            const DomainFlag domain = getDomainFlag(op.get());
            return domain == DomainFlag::none;
        } else if (isComplexNumberHeader(*op.get()))
            return true;
        else if (isNumberHeader(*op.get()))
            return false;
        else if (isConstantHeader(*op.get()))
            // This might change in the future when we want generalised constants with any number
            // type.
            return false;
        else
            // We treat everything else, in particular composites, as complex since the all-of
            // semantics will keep invoking this predicate for all leaves.
            return true;
    });
}

bool sym2::isNumber(ExprView<> e) noexcept
{
    return isNumberHeader(*e.get());
}

bool sym2::isInteger(ExprView<> e) noexcept
{
    return isIntegerHeader(*e.get());
}

bool sym2::isRational(ExprView<> e) noexcept
{
    return isRationalHeader(*e.get());
}

bool sym2::isFloatingPoint(ExprView<> e) noexcept
{
    return isFloatingPointHeader(*e.get());
}

bool sym2::isSmall(ExprView<> e) noexcept
{
    return isSmallHeader(*e.get());
}

bool sym2::isLarge(ExprView<> e) noexcept
{
    return isLargeHeader(*e.get());
}

bool sym2::isScalar(ExprView<> e) noexcept
{
    return isScalarHeader(*e.get());
}

bool sym2::isComposite(ExprView<> e) noexcept
{
    return isCompositeHeader(*e.get());
}

bool sym2::isSymbol(ExprView<> e) noexcept
{
    return isSymbolHeader(*e.get());
}

bool sym2::isConstant(ExprView<> e) noexcept
{
    return isConstantHeader(*e.get());
}

bool sym2::isSum(ExprView<> e) noexcept
{
    return isSumHeader(*e.get());
}

bool sym2::isProduct(ExprView<> e) noexcept
{
    return isProductHeader(*e.get());
}

bool sym2::isPower(ExprView<> e) noexcept
{
    return isPowerHeader(*e.get());
}

bool sym2::isFunction(ExprView<> e) noexcept
{
    return isFunctionHeader(*e.get());
}
