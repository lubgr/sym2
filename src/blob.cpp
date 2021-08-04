
#include "blob.h"
#include <type_traits>

static_assert(sizeof(sym2::Blob) == 16);

static_assert(std::is_trivially_default_constructible_v<sym2::Blob>);
static_assert(std::is_trivially_copyable_v<sym2::Blob>);
static_assert(std::is_trivially_copy_assignable_v<sym2::Blob>);
static_assert(std::is_trivially_move_assignable_v<sym2::Blob>);
static_assert(std::is_trivially_move_constructible_v<sym2::Blob>);
static_assert(std::is_trivially_destructible_v<sym2::Blob>);

sym2::Flag sym2::operator|(Flag lhs, Flag rhs)
{
    return lhs |= rhs;
}

sym2::Flag& sym2::operator|=(Flag& lhs, Flag rhs)
{
    using T = std::underlying_type_t<Flag>;

    lhs = Flag{static_cast<T>(static_cast<T>(lhs) | static_cast<T>(rhs))};

    return lhs;
}

sym2::Flag sym2::operator&(Flag lhs, Flag rhs)
{
    return lhs &= rhs;
}

sym2::Flag& sym2::operator&=(Flag& lhs, Flag rhs)
{
    using T = std::underlying_type_t<Flag>;

    lhs = Flag{static_cast<T>(static_cast<T>(lhs) & static_cast<T>(rhs))};

    return lhs;
}
