
#include "blob.h"
#include <stdexcept>
#include <type_traits>

static_assert(sizeof(sym2::Blob) == 16);
static_assert(alignof(sym2::Blob) == 8);

static_assert(std::is_trivially_default_constructible_v<sym2::Blob>);
static_assert(std::is_trivially_copyable_v<sym2::Blob>);
static_assert(std::is_trivially_copy_assignable_v<sym2::Blob>);
static_assert(std::is_trivially_move_assignable_v<sym2::Blob>);
static_assert(std::is_trivially_move_constructible_v<sym2::Blob>);
static_assert(std::is_trivially_destructible_v<sym2::Blob>);

static constexpr sym2::Blob::Data2 preZero = {.name = {'\0'}};
static constexpr sym2::Blob::Data4 midZero = {.name = {'\0'}};
static constexpr sym2::Blob::Data8 mainZero = {.name = {'\0'}};
static constexpr std::size_t smallNameLength =
  sizeof(sym2::Blob::Data2::name) + sizeof(sym2::Blob::Data4::name) - 1;
static constexpr std::size_t largeNameLength = smallNameLength + sizeof(sym2::Blob::Data8::name);

sym2::Blob sym2::smallIntBlob(std::int32_t n)
{
    return Blob{.header = Type::smallInt,
      .flags = Flag::numericallyEvaluable | Flag::real | (n >= 0 ? Flag::positive : Flag::negative),
      .pre = preZero,
      .mid = midZero,
      .main = {.exact = {n, 1}}};
}

sym2::Blob sym2::floatingPointBlob(double n)
{
    return Blob{.header = Type::floatingPoint,
      .flags =
        Flag::numericallyEvaluable | Flag::real | (n >= 0.0 ? Flag::positive : Flag::negative),
      .pre = preZero,
      .mid = midZero,
      .main = {.inexact = n}};
}

sym2::Blob sym2::symbolBlob(std::string_view symbol)
{
    Blob result{.header = Type::symbol,
      .flags = Flag::none,
      .pre = preZero,
      .mid = midZero,
      .main = mainZero};

    auto* nameDest = std::next(reinterpret_cast<char*>(&result), 2);

    if (symbol.length() > largeNameLength || symbol.empty())
        throw std::invalid_argument("Names must be non-empty and < 13 characters long");

    std::copy(symbol.cbegin(), symbol.cend(), nameDest);

    return result;
}

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
