
#include "query.h"
#include <cassert>

sym2::Type sym2::type(ExprView e)
{
    return e[0].header;
}

sym2::Sign sym2::sign(ExprView e)
{
    return e[0].sign;
}

sym2::Flag sym2::flags(ExprView e)
{
    return e[0].flags;
}

bool sym2::isScalar(ExprView e)
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

bool sym2::isNumber(ExprView e)
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

bool sym2::isInteger(ExprView e)
{
    switch (type(e)) {
        case Type::smallInt:
        case Type::largeInt:
            return true;
        default:
            return false;
    }
}

bool sym2::isSymbolOrConstant(ExprView e)
{
    return isSymbol(e) || isConstant(e);
}

bool sym2::isSymbol(ExprView e)
{
    return type(e) == Type::symbol;
}

bool sym2::isConstant(ExprView e)
{
    return type(e) == Type::constant;
}

bool sym2::isNumericallyEvaluable(ExprView e)
{
    return (flags(e) & Flag::numericallyEvaluable) != Flag::none;
}

std::size_t sym2::nOps(ExprView e)
{
    if (isScalar(e))
        return 0;

    return e[0].data.count;
}
