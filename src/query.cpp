
#include "query.h"
#include <cassert>
#include "childiterator.h"
#include "expr.h"
#include "get.h"
#include "predicates.h"
#include "view.h"

sym2::Type sym2::type(ExprView<> e)
{
    return e[0].header;
}

sym2::Flag sym2::flags(ExprView<> e)
{
    return e[0].flags;
}

sym2::BaseExp sym2::asPower(ExprView<> e)
{
    static const auto one = 1_ex;

    if (is<power>(e))
        return {firstOperand(e), secondOperand(e)};

    return {e, one};
}

std::size_t sym2::nOperands(ExprView<> e)
{
    return nOperands(e[0]);
}

std::size_t sym2::nOperands(Blob b)
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
            return b.mid.nLogicalOrPhysicalChildren;
    }
}

std::size_t sym2::nPhysicalChildren(ExprView<> e)
{
    return nPhysicalChildren(e[0]);
}

std::size_t sym2::nPhysicalChildren(Blob b)
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

sym2::ExprView<> sym2::nthOperand(ExprView<> e, std::uint32_t n)
{
    auto operand = ChildIterator::logicalChildren(e);

    assert(n > 0);

    return *std::next(operand, n - 1);
}

sym2::ExprView<> sym2::firstOperand(ExprView<> e)
{
    return nthOperand(e, 1);
}

sym2::ExprView<> sym2::secondOperand(ExprView<> e)
{
    return nthOperand(e, 2);
}

namespace sym2 {
    namespace {
        sym2::ExprView<> nthPhysical(ExprView<> e, std::uint32_t n)
        {
            auto operand = ChildIterator::physicalChildren(e);

            assert(n > 0);

            return *std::next(operand, n - 1);
        }
    }
}

sym2::ExprView<sym2::number> sym2::real(ExprView<complexDomain> c)
{
    return nthPhysical(c, 1);
}

sym2::ExprView<sym2::number> sym2::imag(ExprView<complexDomain> c)
{
    return nthPhysical(c, 2);
}

sym2::ExprView<sym2::integer> sym2::numerator(ExprView<rational> n)
{
    return nthPhysical(n, 1);
}

sym2::ExprView<sym2::integer> sym2::denominator(ExprView<rational> n)
{
    return nthPhysical(n, 2);
}
