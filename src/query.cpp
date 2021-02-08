
#include "query.h"
#include <cassert>
#include "expr.h"
#include "get.h"
#include "view.h"

sym2::Type sym2::type(ExprView e)
{
    return e[0].header;
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

bool sym2::isRealDomainNumber(ExprView e)
{
    return isNumber(e) && !isComplexNumber(e);
}

bool sym2::isInteger(ExprView e)
{
    return isSmallInt(e) || isLargeInt(e);
}

bool sym2::isSmallInt(ExprView e)
{
    return type(e) == Type::smallInt;
}

bool sym2::isLargeInt(ExprView e)
{
    return type(e) == Type::largeInt;
}

bool sym2::isRational(ExprView e)
{
    return isSmallRational(e) || isLargeRational(e);
}

bool sym2::isSmallRational(ExprView e)
{
    return type(e) == Type::smallRational;
}

bool sym2::isLargeRational(ExprView e)
{
    return type(e) == Type::largeRational;
}

bool sym2::isDouble(ExprView e)
{
    return type(e) == Type::floatingPoint;
}

bool sym2::isComplexNumber(ExprView e)
{
    return type(e) == Type::complexNumber;
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

bool sym2::isSum(ExprView e)
{
    return type(e) == Type::sum;
}

bool sym2::isProduct(ExprView e)
{
    return type(e) == Type::product;
}

bool sym2::isPower(ExprView e)
{
    return type(e) == Type::power;
}

bool sym2::isFunction(ExprView e)
{
    return type(e) == Type::function;
}

bool sym2::isFunction(ExprView e, std::string_view name)
{
    return isFunction(e) && name == get<std::string_view>(e);
}

bool sym2::isNumericallyEvaluable(ExprView e)
{
    return (flags(e) & Flag::numericallyEvaluable) != Flag::none;
}

std::size_t sym2::nLogicalOperands(ExprView e)
{
    return nLogicalOperands(e[0]);
}

std::size_t sym2::nLogicalOperands(Blob b)
{
    switch (b.header) {
        case Type::smallInt:
        case Type::smallRational:
        case Type::floatingPoint:
        case Type::largeInt:
        case Type::largeRational:
        case Type::symbol:
        case Type::constant:
            return 0;
        default:
            return b.mid.nLogicalOperands;
    }
}

std::size_t sym2::nChildBlobs(ExprView e)
{
    return nChildBlobs(e[0]);
}

std::size_t sym2::nChildBlobs(Blob b)
{
    switch (b.header) {
        case Type::smallInt:
        case Type::smallRational:
        case Type::floatingPoint:
        case Type::symbol:
        case Type::constant:
            return 0;
        default:
            return b.main.nChildBlobs;
    }
}

sym2::ExprView sym2::nth(ExprView e, std::uint32_t n)
{
    OperandIterator operand{e};

    assert(n > 0);

    std::advance(operand, n - 1);

    return *operand;
}

sym2::ExprView sym2::first(ExprView e)
{
    return nth(e, 1);
}

sym2::ExprView sym2::second(ExprView e)
{
    return nth(e, 2);
}
