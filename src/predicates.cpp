
#include "predicates.h"
#include "query.h"

bool sym2::isRealDomain(ExprView<> e)
{
    // TODO This is incomplete, we need a tag for symbols to be marked real
    switch (type(e)) {
        case Type::smallInt:
        case Type::smallRational:
        case Type::floatingPoint:
        case Type::largeInt:
        case Type::largeRational:
            return true;
        default:
            return false;
    }
}

bool sym2::isComplexDomain(ExprView<> e)
{
    // TODO This is incomplete, we need a tag for symbols to be marked complex
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
