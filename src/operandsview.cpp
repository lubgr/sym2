
#include "operandsview.h"
#include <boost/range/algorithm/equal.hpp>
#include <cassert>
#include "query.h"

sym2::OperandsView::OperandsView(ChildIterator first) noexcept
    : first{first}
{}

sym2::OperandsView sym2::OperandsView::operandsOf(ExprView<> e) noexcept
{
    return OperandsView{ChildIterator::logicalChildren(e)};
}

sym2::OperandsView sym2::OperandsView::singleOperand(ExprView<> e) noexcept
{
    return OperandsView{ChildIterator::singleChild(e)};
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
    return first.n;
}

sym2::OperandsView sym2::OperandsView::subview(std::size_t offset, std::size_t count) const noexcept
{
    constexpr std::size_t npos = -1;
    OperandsView result{};

    assert(offset <= size());
    assert(count == npos || offset + count <= size());

    result.first = std::next(first, offset);
    result.sentinel = count == npos ? sentinel : std::next(result.first, count);

    return result;
}

bool sym2::OperandsView::operator==(OperandsView rhs) const noexcept
{
    // TODO when this class saves the number of physical blobs upon construction, we can turn this
    // into a memcmp:
    return boost::equal(*this, rhs);
}

bool sym2::OperandsView::operator!=(OperandsView rhs) const noexcept
{
    return !(*this == rhs);
}
