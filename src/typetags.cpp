
#include "typetags.h"
#include "predicates.h"
#include "query.h"

template <>
bool sym2::is<sym2::Real>(ExprView<> e)
{
    return isRealDomain(e);
}

template <>
bool sym2::is<sym2::Complex>(ExprView<> e)
{
    return isComplexDomain(e);
}

template <>
bool sym2::is<sym2::Number>(ExprView<> e)
{
    return isNumber(e);
}

template <>
bool sym2::is<sym2::Int>(ExprView<> e)
{
    return isInteger(e);
}

template <>
bool sym2::is<sym2::Rational>(ExprView<> e)
{
    return isRational(e);
}

template <>
bool sym2::is<sym2::Double>(ExprView<> e)
{
    return isFloatingPoint(e);
}

template <>
bool sym2::is<sym2::Small>(ExprView<> e)
{
    return isSmall(e);
}

template <>
bool sym2::is<sym2::Large>(ExprView<> e)
{
    return isLarge(e);
}

template <>
bool sym2::is<sym2::Scalar>(ExprView<> e)
{
    return isScalar(e);
}

template <>
bool sym2::is<sym2::Symbol>(ExprView<> e)
{
    return isSymbol(e);
}

template <>
bool sym2::is<sym2::Constant>(ExprView<> e)
{
    return isConstant(e);
}

template <>
bool sym2::is<sym2::Sum>(ExprView<> e)
{
    return isSum(e);
}

template <>
bool sym2::is<sym2::Product>(ExprView<> e)
{
    return isProduct(e);
}

template <>
bool sym2::is<sym2::Power>(ExprView<> e)
{
    return isPower(e);
}

template <>
bool sym2::is<sym2::Function>(ExprView<> e)
{
    return isFunction(e);
}

template <>
bool sym2::is<sym2::NumericallyEvaluable>(ExprView<> e)
{
    return isNumericallyEvaluable(e);
}
