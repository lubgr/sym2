
#include "operandsview.h"
#include <boost/range/algorithm/equal.hpp>
#include <cassert>
#include "sym2/query.h"

sym2::OperandsView::OperandsView(const ChildIterator first, const ChildIterator last) noexcept
    : first{first}
    , sentinel{last}
{}

sym2::OperandsView sym2::OperandsView::operandsOf(const ExprView<> e) noexcept
{
    return OperandsView{
      ChildIterator::logicalChildren(e), ChildIterator::logicalChildrenSentinel(e)};
}

sym2::OperandsView sym2::OperandsView::singleOperand(const ExprView<> e) noexcept
{
    return OperandsView{ChildIterator::singleChild(e), ChildIterator::singleChildSentinel(e)};
}

sym2::ChildIterator sym2::OperandsView::begin() const noexcept
{
    return first;
}

sym2::ChildIterator sym2::OperandsView::end() const noexcept
{
    return sentinel;
}

std::size_t sym2::OperandsView::size() const noexcept
{
    return static_cast<std::size_t>(sentinel - first);
}

bool sym2::OperandsView::empty() const noexcept
{
    return first == sentinel;
}

sym2::OperandsView sym2::OperandsView::subview(
  const std::size_t offset, const std::size_t count) const noexcept
{
    constexpr std::size_t npos = -1;
    OperandsView result{};

    assert(offset <= size());
    assert(count == npos || offset + count <= size());

    result.first = std::next(first, offset);
    result.sentinel = count == npos ? sentinel : std::next(result.first, count);

    return result;
}

bool sym2::OperandsView::operator==(const OperandsView rhs) const noexcept
{
    return boost::equal(*this, rhs);
}

bool sym2::OperandsView::operator!=(const OperandsView rhs) const noexcept
{
    return !(*this == rhs);
}
