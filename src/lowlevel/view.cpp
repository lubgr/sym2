
#include "view.h"
#include <cstring>
#include "blob.h"

static_assert(std::is_trivially_copyable_v<sym2::ExprView<>>);
static_assert(std::is_trivially_copy_assignable_v<sym2::ExprView<>>);
static_assert(std::is_trivially_move_constructible_v<sym2::ExprView<>>);
static_assert(std::is_trivially_move_assignable_v<sym2::ExprView<>>);

sym2::ConstBlobIterator::ConstBlobIterator(const Blob* b) noexcept
    : b{b}
{}

const sym2::Blob& sym2::ConstBlobIterator::operator*() const noexcept
{
    return *b;
}

sym2::ConstBlobIterator& sym2::ConstBlobIterator::operator+=(std::ptrdiff_t n) noexcept
{
    b += n;
    return *this;
}

sym2::ConstBlobIterator::difference_type sym2::ConstBlobIterator::operator-(
  ConstBlobIterator other) const noexcept
{
    return b - other.b;
}

const sym2::Blob* sym2::detail::nextBlobHelper(const Blob* current, std::size_t n)
{
    return std::next(current, n);
}

bool sym2::operator==(ExprView<> lhs, ExprView<> rhs)
{
    if (lhs.size() != rhs.size())
        return false;
    else
        return std::memcmp(&lhs.front(), &rhs.front(), lhs.size() * sizeof(Blob)) == 0;
}

bool sym2::operator!=(ExprView<> lhs, ExprView<> rhs)
{
    return !(lhs == rhs);
}
