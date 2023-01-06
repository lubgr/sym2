
#include "childiterator.h"
#include "blob.h"

sym2::ChildIterator::ChildIterator(const Blob* op)
    : op{op}
{}

sym2::ChildIterator sym2::ChildIterator::logicalChildren(ExprView<> e) noexcept
{
    return ChildIterator{getFirstOperand(e.get())};
}

sym2::ChildIterator sym2::ChildIterator::ChildIterator::singleChild(ExprView<> e) noexcept
{
    return ChildIterator{e.get()};
}

sym2::ChildIterator sym2::ChildIterator::ChildIterator::logicalChildrenSentinel(
  ExprView<> e) noexcept
{
    return ChildIterator{getPastTheEndOperand(e.get())};
}

sym2::ChildIterator sym2::ChildIterator::ChildIterator::singleChildSentinel(ExprView<> e) noexcept
{
    return ChildIterator{std::next(e.get())};
}

sym2::ExprView<> sym2::ChildIterator::operator*() const noexcept
{
    return ExprView<>{op};
}

sym2::ChildIterator::difference_type sym2::ChildIterator::operator-(
  ChildIterator rhs) const noexcept
{
    return op - rhs.op;
}

sym2::ChildIterator sym2::ChildIterator::operator+=(std::size_t n) noexcept
{
    op += n;

    return *this;
}
