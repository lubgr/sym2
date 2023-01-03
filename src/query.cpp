
#include "sym2/query.h"
#include <cassert>
#include <functional>
#include <ranges>
#include "childiterator.h"
#include "sym2/expr.h"
#include "sym2/exprview.h"
#include "sym2/get.h"
#include "lowlevel/blobapi.h"
#include "operandsview.h"
#include "sym2/predicates.h"

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

sym2::ExprView<> sym2::firstOperand(ExprView<!small> e)
{
    return nthOperand(e, 0);
}

sym2::ExprView<> sym2::secondOperand(ExprView<!small> e)
{
    return nthOperand(e, 1);
}

std::size_t sym2::nOperands(ExprView<> e)
{
    return nOperands(e.get());
}

sym2::ExprView<> sym2::nthOperand(ExprView<!small> e, std::uint16_t n)
{
    assert(static_cast<std::size_t>(n + 1) <= nOperands(e));

    return *std::next(ChildIterator::logicalChildren(e), n);
}

sym2::ExprView<sym2::number> sym2::real(ExprView<number> n)
{
    if (is<complexDomain>(n))
        return ExprView<>{getRealFromCommplexNumber(n.get())};
    else
        return n;
}

sym2::ExprView<sym2::number> sym2::imag(ExprView<number> n)
{
    static auto zero = 0_ex;

    if (is<complexDomain>(n))
        return ExprView<>{getImagFromCommplexNumber(n.get())};
    else
        return zero;
}

std::pair<sym2::ExprView<>, sym2::OperandsView> sym2::frontAndRest(OperandsView ops)
{
    assert(ops.size() >= 1);
    return std::make_pair(*ops.begin(), ops.subview(1));
}

std::pair<sym2::ExprView<>, std::span<const sym2::Expr>> sym2::frontAndRest(
  std::span<const Expr> ops)
{
    assert(ops.size() >= 1);
    return std::make_pair(std::ref(*ops.begin()), ops.subspan(1));
}

std::pair<sym2::ExprView<>, std::span<const sym2::ExprView<>>> sym2::frontAndRest(
  std::span<const ExprView<>> ops)
{
    assert(ops.size() >= 1);
    return std::make_pair(*ops.begin(), ops.subspan(1));
}

bool sym2::contains(ExprView<> needle, ExprView<> haystack)
{
    if (haystack == needle)
        return true;
    else if (is<composite>(haystack)) {
        const OperandsView ops = OperandsView::operandsOf(haystack);
        return std::any_of(ops.begin(), ops.end(), std::bind_front(&contains, needle));
    } else
        return false;
}
