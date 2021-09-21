
#include "operands.h"
#include <cassert>
#include "query.h"

sym2::OperandIterator::OperandIterator(const Blob* op, std::size_t n)
    : op{op}
    , n{n}
{}

sym2::OperandIterator sym2::OperandIterator::fromComposite(ExprView<> e) noexcept
{
    assert(e.size() >= 1);

    const std::size_t n = nLogicalOperands(e);
    const Blob* const start = type(e) == Type::function ? &e[2] : &e[1];

    return OperandIterator{start, n};
}

sym2::OperandIterator sym2::OperandIterator::fromCompositeOrSingle(ExprView<> e) noexcept
{
    return nChildBlobs(e) == 0 ? fromSingle(e) : fromComposite(e);
}

sym2::OperandIterator sym2::OperandIterator::OperandIterator::fromSingle(ExprView<> e) noexcept
{
    return OperandIterator{&e[0], 1};
}

sym2::OperandIterator sym2::OperandIterator::fromSequence(std::span<const Expr> expressions) noexcept
{
    if (expressions.empty())
        return OperandIterator{};
    else {
        ExprView<> first = expressions.front();
        return OperandIterator{&first[0], expressions.size()};
    }
}

sym2::ExprView<> sym2::OperandIterator::operator*() const noexcept
{
    return {op, currentSize()};
}

sym2::OperandIterator& sym2::OperandIterator::operator++() noexcept
{
    assert(op != nullptr);

    if (--n == 0)
        op = nullptr;
    else
        op = op + currentSize();

    return *this;
}

std::size_t sym2::OperandIterator::currentSize() const noexcept
{
    return nChildBlobs(*op) + 1;
}

sym2::OperandsView::OperandsView(OperandIterator first) noexcept
    : first{first}
{}

sym2::OperandsView sym2::OperandsView::fromComposite(ExprView<> e) noexcept
{
    return OperandsView{OperandIterator::fromComposite(e)};
}

sym2::OperandsView sym2::OperandsView::fromSingle(ExprView<> e) noexcept
{
    return OperandsView{OperandIterator::fromSingle(e)};
}

sym2::OperandsView sym2::OperandsView::fromCompositeOrSingle(ExprView<> e) noexcept
{
    return OperandsView{OperandIterator::fromCompositeOrSingle(e)};
}

sym2::OperandsView sym2::OperandsView::fromSequence(std::span<const Expr> expressions) noexcept
{
    return OperandsView{OperandIterator::fromSequence(expressions)};
}

sym2::OperandIterator sym2::OperandsView::begin() const noexcept
{
    return first;
}

sym2::OperandIterator sym2::OperandsView::end() const noexcept
{
    return sentinel;
}

std::size_t sym2::OperandsView::size() const noexcept
{
    return first.n;
}

sym2::OperandsView sym2::OperandsView::subview(std::size_t offset, std::size_t count) const noexcept
{
    constexpr std::size_t npos = -1;
    OperandsView result{};

    assert(offset < size());
    assert(count == npos || offset + count <= size());

    result.first = std::next(first, offset);
    result.sentinel = count == npos ? sentinel : std::next(result.first, count);

    return result;
}
