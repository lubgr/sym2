
#include "childiterator.h"
#include "blob.h"
#include "access.h"
#include "query.h"

sym2::ChildIterator::ChildIterator(const Blob* op, std::size_t n)
    : op{op}
    , n{n}
{}

sym2::ChildIterator sym2::ChildIterator::logicalChildren(ExprView<> e) noexcept
{
    assert(e.size() >= 1);

    const std::size_t n = nOperands(e);
    const Blob* const start = type(e) == Type::function ? &e[2] : &e[1];

    return ChildIterator{start, n};
}

sym2::ChildIterator sym2::ChildIterator::physicalChildren(ExprView<> e) noexcept
{
    const std::size_t n = nPhysicalChildren(e);

    assert(n >= 1);

    const Blob* const start = type(e) == Type::function ? &e[2] : &e[1];

    return ChildIterator{start, n};
}

sym2::ChildIterator sym2::ChildIterator::ChildIterator::singleChild(ExprView<> e) noexcept
{
    return ChildIterator{&e[0], 1};
}

sym2::ExprView<> sym2::ChildIterator::operator*() const noexcept
{
    return {op, currentSize()};
}

sym2::ChildIterator& sym2::ChildIterator::operator++() noexcept
{
    assert(op != nullptr);

    if (--n == 0)
        op = nullptr;
    else
        op = op + currentSize();

    return *this;
}

std::size_t sym2::ChildIterator::currentSize() const noexcept
{
    return nPhysicalChildren(*op) + 1;
}
