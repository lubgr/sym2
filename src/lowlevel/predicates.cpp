
#include "predicates.h"
#include "expr.h"
#include "blob.h"
#include "query.h"
#include "access.h"

bool sym2::isRealDomain(ExprView<> e)
{
    switch (type(e)) {
        case Type::smallInt:
        case Type::smallRational:
        case Type::floatingPoint:
        case Type::largeInt:
        case Type::largeRational:
            return true;
        case Type::symbol:
            return (flags(e) & Flag::real) != Flag::none;
        default:
            return false;
    }
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
    switch (type(e)) {
        case Type::sum:
        case Type::product:
        case Type::power:
        case Type::function:
            return false;
        default:
            return true;
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
    switch (type(e)) {
        case Type::symbol:
            return (flags(e) & Flag::positive) != Flag::none;
        default:
            // TODO
            return true;
    }
}

bool sym2::isNegative(ExprView<>)
{
    // TODO
    return false;
}

bool sym2::isZero(ExprView<> e)
{
    static const Expr zero{0};

    return e == zero;
}

bool sym2::isOne(ExprView<> e)
{
    static const Expr one{1};

    return e == one;
}
