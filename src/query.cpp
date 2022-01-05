
#include "query.h"
#include <cassert>
#include "childiterator.h"
#include "expr.h"
#include "get.h"
#include "predicates.h"
#include "view.h"

sym2::BaseExp sym2::splitAsPower(ExprView<> e)
{
    static const auto one = 1_ex;

    if (is<power>(e))
        return {firstOperand(e), secondOperand(e)};

    return {e, one};
}

sym2::ConstAndTerm sym2::splitConstTerm(ExprView<!number> e)
{
    static const auto one = 1_ex;

    if (is<product>(e)) {
        const OperandsView ops = OperandsView::operandsOf(e);
        const auto [first, rest] = frontAndRest(ops);

        if (is<number>(first))
            return {first, rest};
        else
            return {one, ops};
    }

    assert(is < symbol || constant || sum || power || function > (e));

    return {one, OperandsView::singleOperand(e)};
}

sym2::ExprView<> sym2::nthOperand(ExprView<composite> e, std::uint32_t n)
{
    auto operand = ChildIterator::logicalChildren(e);

    assert(n > 0);

    return *std::next(operand, n - 1);
}

sym2::ExprView<> sym2::firstOperand(ExprView<composite> e)
{
    return nthOperand(e, 1);
}

sym2::ExprView<> sym2::secondOperand(ExprView<composite> e)
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

sym2::ExprView<sym2::number> sym2::real(ExprView<number> c)
{
    if (is<complexDomain>(c))
        return nthPhysical(c, 1);
    else
        return c;
}

sym2::ExprView<sym2::number> sym2::imag(ExprView<number> c)
{
    static const Expr zero{0};

    if (is<complexDomain>(c))
        return nthPhysical(c, 2);
    else
        return zero;
}

sym2::ExprView<sym2::integer> sym2::numerator(ExprView<rational> n)
{
    return nthPhysical(n, 1);
}

sym2::ExprView<sym2::integer> sym2::denominator(ExprView<rational> n)
{
    return nthPhysical(n, 2);
}

std::pair<sym2::ExprView<>, sym2::OperandsView> sym2::frontAndRest(OperandsView ops)
{
    assert(ops.size() >= 1);
    return std::make_pair(ops.front(), ops.subview(1));
}

std::pair<sym2::ExprView<>, std::span<const sym2::Expr>> sym2::frontAndRest(std::span<const Expr> ops)
{
    assert(ops.size() >= 1);
    return std::make_pair(std::ref(ops.front()), ops.subspan(1));
}

std::pair<sym2::ExprView<>, std::span<const sym2::ExprView<>>> sym2::frontAndRest(std::span<const ExprView<>> ops)
{
    assert(ops.size() >= 1);
    return std::make_pair(ops.front(), ops.subspan(1));
}
