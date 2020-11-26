
#include "expr.h"
#include <algorithm>
#include <boost/iterator/function_output_iterator.hpp>
#include <cstring>
#include <limits>
#include <numeric>
#include <stdexcept>
#include <type_traits>
#include "query.h"

static_assert(std::is_trivial_v<sym2::Operand>);

namespace sym2 {
    namespace {
        template <class Number>
        Sign numberSign(const Number& n)
        {
            if (n > 0)
                return Sign::positive;
            else if (n < 0)
                return Sign::negative;

            return Sign::neither;
        }
    }
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

sym2::Expr::Expr(int n)
{
    appendSmallInt(n);
}

sym2::Expr::Expr(double n)
    : buffer{Operand{.header = Type::floatingPoint,
      .sign = numberSign(n),
      .flags = Flag::numericallyEvaluable,
      .name = {'\0'},
      .data = {.inexact = n}}}
{}

sym2::Expr::Expr(int num, int denom)
{
    if (denom == 0)
        throw std::domain_error("Zero denominator of small exact rational");

    if (denom < 0) {
        denom = -denom;
        num = -num;
    }

    appendSmallRationalOrInt(num, denom);
}

sym2::Expr::Expr(const Int& n)
{
    appendSmallOrLargeInt(n);
}

sym2::Expr::Expr(const Rational& n)
{
    const auto num = numerator(n);
    const auto denom = denominator(n);

    if (fitsInto<std::int32_t>(num) && fitsInto<std::int32_t>(denom)) {
        appendSmallRationalOrInt(static_cast<std::int32_t>(num), static_cast<std::int32_t>(denom));
        return;
    }

    buffer.push_back(Operand{.header = Type::largeRational,
      .sign = numberSign(n),
      .flags = Flag::numericallyEvaluable,
      .name = {'\0'},
      .data = {.count = 2}});

    appendSmallOrLargeInt(num);
    appendSmallOrLargeInt(denom);
}

sym2::Expr::Expr(std::string_view symbol)
{
    if (symbol.length() > 13 || symbol.empty())
        throw std::invalid_argument("Symbol names must be non-empty and < 13 characters long");

    Operand op{
      .header = Type::symbol, .sign = Sign::neither, .flags = Flag::none, .name = {'\0'}, .data = {.name = {'\0'}}};

    if (symbol == "pi" || symbol == "euler") {
        op.header = Type::constant;
        op.sign = Sign::positive;
        op.flags = Flag::numericallyEvaluable;
    }

    if (symbol[0] == '+')
        op.sign = Sign::positive;

    auto* dest = std::next(reinterpret_cast<char*>(&op), 2);

    std::copy(symbol.cbegin(), symbol.cend(), dest);

    buffer.push_back(op);
}

sym2::Expr::Expr(ExprView e)
    : buffer{e.begin(), e.end()}
{}

sym2::Expr::Expr(Type composite, std::span<const ExprView> ops)
    : buffer{Operand{
      .header = composite, .sign = Sign::unknown, .flags = Flag::none, .name = {'\0'}, .data = {.count = ops.size()}}}
{
    assert(
      composite == Type::sum || composite == Type::product || composite == Type::power || composite == Type::function);

    /* Likely to be more, but we also don't premature allocation if it might just fit in-place: */
    buffer.reserve(ops.size());

    bool numEval = true;

    for (ExprView ev : ops) {
        numEval = numEval && isNumericallyEvaluable(ev);
        buffer.insert(buffer.end(), ev.begin(), ev.end());
    }

    if (numEval)
        buffer.front().flags = Flag::numericallyEvaluable;
}

sym2::Expr::Expr(Type composite, std::initializer_list<ExprView> ops)
    : Expr{composite, std::span<const ExprView>{ops.begin(), ops.end()}}
{}

void sym2::Expr::appendSmallInt(std::int32_t n)
{
    buffer.push_back(Operand{.header = Type::smallInt,
      .sign = numberSign(n),
      .flags = Flag::numericallyEvaluable,
      .name = {'\0'},
      .data = {.exact = {n, 1}}});
}

void sym2::Expr::appendSmallRationalOrInt(std::int32_t num, std::int32_t denom)
{
    const auto divisor = std::gcd(num, denom);

    num /= divisor;
    denom /= divisor;

    if (denom == 1)
        appendSmallInt(num);
    else
        buffer.push_back(Operand{.header = Type::smallRational,
          .sign = numberSign(static_cast<double>(num) / denom),
          .flags = Flag::numericallyEvaluable,
          .name = {'\0'},
          .data = {.exact = {num, denom}}});
}

void sym2::Expr::appendSmallOrLargeInt(const Int& n)
{
    if (fitsInto<std::int32_t>(n))
        appendSmallInt(static_cast<std::int32_t>(n));
    else
        appendLargeInt(n);
}

void sym2::Expr::appendLargeInt(const Int& n)
{
    static constexpr auto opSize = sizeof(Operand);

    buffer.push_back(Operand{.header = Type::largeInt,
      .sign = numberSign(n),
      .flags = Flag::numericallyEvaluable,
      .name = {'\0'},
      .data = {.count = 0 /* Not known yet. */}});

    /* Save the index instead of a reference to back(), which might be invalidated below. */
    const std::size_t frontIdx = buffer.size() - 1;

    std::size_t byteCount = 0;
    const auto toCountedAliasedOps = [&byteCount, this](unsigned char byte) mutable {
        const auto opByteIndex = byteCount % opSize;

        ++byteCount;

        if (opByteIndex == 0) {
            buffer.push_back({});
            std::memset(&buffer.back(), 0, opSize);
        }

        auto* aliased = reinterpret_cast<unsigned char*>(&buffer.back());
        aliased[opByteIndex] = byte;
    };

    export_bits(n, boost::make_function_output_iterator(toCountedAliasedOps), 8);

    const auto remainder = byteCount % opSize;

    auto* aliased = reinterpret_cast<unsigned char*>(&buffer[frontIdx + 1]);
    assert(buffer.size() > frontIdx + 1);
    const auto length = buffer.size() - frontIdx - 1;

    buffer[frontIdx].data.count = length;

    /* Rotate any trailing zero bytes to the front. When import and exports of large integer bits happens with the
     * most significant bits first, leading zeros are dropped. This allows for an easier import, as the whole
     * Operand span can be used as the bit source: */
    std::rotate(aliased, aliased + length * opSize - (opSize - remainder), aliased + length * opSize);
}

sym2::Expr::operator sym2::ExprView() const
{
    assert(buffer.size() >= 1);

    return ExprView{buffer.data(), buffer.size()};
}

bool sym2::operator==(ExprView lhs, ExprView rhs)
{
    if (lhs.size() != rhs.size())
        return false;
    else
        return std::memcmp(&lhs.front(), &rhs.front(), lhs.size() * sizeof(Operand)) == 0;
}

bool sym2::operator!=(ExprView lhs, ExprView rhs)
{
    return !(lhs == rhs);
}

sym2::Expr sym2::operator"" _ex(const char* str, std::size_t)
{
    return Expr{str};
}

sym2::Expr sym2::operator"" _ex(unsigned long long n)
{
    if (n > std::numeric_limits<int>::max())
        throw std::domain_error("Integral Expr literals must fit into an int");

    return Expr{static_cast<int>(n)};
}
