
#include "query.h"
#include <cassert>
#include "expr.h"
#include "get.h"
#include "typetags.h"
#include "view.h"

sym2::Type sym2::type(ExprView e)
{
    return e[0].header;
}

sym2::Flag sym2::flags(ExprView e)
{
    return e[0].flags;
}

sym2::ExprView sym2::base(ExprView e)
{
    if (is<Power>(e))
        return first(e);

    return e;
}

sym2::ExprView sym2::exponent(ExprView e)
{
    static const auto one = 1_ex;

    if (is<Power>(e))
        return second(e);

    return one;
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
