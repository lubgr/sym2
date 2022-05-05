
#include "predicates.h"
#include <functional>
#include "access.h"
#include "blob.h"
#include "expr.h"
#include "get.h"
#include "operandsview.h"
#include "query.h"

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
    }
}

bool sym2::isRealDomain(ExprView<> e)
{
    return (flags(e) & Flag::real) != Flag::none;
}

bool sym2::isComplexDomain(ExprView<> e)
{
    return type(e) == Type::complexNumber;
}

bool sym2::isNumber(ExprView<> e)
{
    switch (type(e)) {
        case Type::smallInt:
        case Type::smallRational:
        case Type::floatingPoint:
        case Type::largeInt:
        case Type::largeRational:
        case Type::complexNumber:
            return true;
        default:
            return false;
    }
}

bool sym2::isInteger(ExprView<> e)
{
    const Type t = type(e);

    return t == Type::smallInt || t == Type::largeInt;
}

bool sym2::isRational(ExprView<> e)
{
    switch (type(e)) {
        case Type::smallInt:
        case Type::smallRational:
        case Type::largeInt:
        case Type::largeRational:
            return true;
        default:
            return false;
    }
}

bool sym2::isFloatingPoint(ExprView<> e)
{
    return type(e) == Type::floatingPoint;
}

bool sym2::isSmall(ExprView<> e)
{
    switch (type(e)) {
        case Type::smallInt:
        case Type::smallRational:
        case Type::symbol:
        case Type::floatingPoint:
        case Type::constant:
            return true;
        default:
            return false;
    }
}

bool sym2::isLarge(ExprView<> e)
{
    return !isSmall(e);
}

bool sym2::isScalar(ExprView<> e)
{
    return !isComposite(e);
}

bool sym2::isComposite(ExprView<> e)
{
    switch (type(e)) {
        case Type::sum:
        case Type::product:
        case Type::power:
        case Type::function:
            return true;
        default:
            return false;
    }
}

bool sym2::isSymbol(ExprView<> e)
{
    return type(e) == Type::symbol;
}

bool sym2::isConstant(ExprView<> e)
{
    return type(e) == Type::constant;
}

bool sym2::isSum(ExprView<> e)
{
    return type(e) == Type::sum;
}

bool sym2::isProduct(ExprView<> e)
{
    return type(e) == Type::product;
}

bool sym2::isPower(ExprView<> e)
{
    return type(e) == Type::power;
}

bool sym2::isFunction(ExprView<> e)
{
    return type(e) == Type::function;
}

bool sym2::isNumericallyEvaluable(ExprView<> e)
{
    return (flags(e) & Flag::numericallyEvaluable) != Flag::none;
}

bool sym2::isPositive(ExprView<> e)
{
    if ((flags(e) & Flag::positive) != Flag::none)
        return true;
    else if (isNumericallyEvaluable(e))
        return get<double>(e) >= 0.0;
    else if (isSum(e)) {
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

bool sym2::isNegative(ExprView<> e)
{
    if ((flags(e) & Flag::negative) != Flag::none)
        return true;
    else if (isNumericallyEvaluable(e))
        return get<double>(e) < 0.0;
    else if (isSum(e)) {
        const NonNumericSign sign = signOfNonNumericallyEvaluable(e);
        if (sign == NonNumericSign::onlyNumeric)
            return reduceNumericallyEvaluable(e, std::plus<>{}, 0.0) < 0.0;
        else if (sign == NonNumericSign::negative)
            return reduceNumericallyEvaluable(e, std::plus<>{}, 0.0) <= 0.0;
    } else if (isProduct(e))
        return signOfProduct(e) == -1;

    return false;
}

bool sym2::isZero(ExprView<> e)
{
    return e == 0_ex;
}

bool sym2::isOne(ExprView<> e)
{
    return e == 1_ex;
}
