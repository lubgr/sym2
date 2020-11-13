
#include "query.h"
#include <cassert>

sym2::Flag sym2::flag(ExprView e)
{
    return e[0].header;
}

bool sym2::isScalar(ExprView e)
{
    switch (flag(e)) {
        case Flag::sum:
        case Flag::product:
        case Flag::power:
        case Flag::function:
            return false;
        default:
            return true;
    }
}

bool sym2::isNumber(ExprView e)
{
    switch (flag(e)) {
        case Flag::smallInt:
        case Flag::smallRational:
        case Flag::floatingPoint:
        case Flag::largeRational:
        case Flag::complexNumber:
            return true;
        default:
            return false;
    }
}

bool sym2::isInteger(ExprView e)
{
    switch (flag(e)) {
        case Flag::smallInt:
            return true;
        case Flag::largeRational:
            return e.front().data.large->denominator() == 1;
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
    return flag(e) == Flag::symbol;
}

bool sym2::isConstant(ExprView e)
{
    return flag(e) == Flag::constant;
}

std::size_t sym2::nOps(ExprView e)
{
    if (isScalar(e))
        return 0;

    return e[0].data.count;
}
