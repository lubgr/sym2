
#include "expr.h"
#include <algorithm>
#include <boost/iterator/function_output_iterator.hpp>
#include <boost/range/algorithm.hpp>
#include <cstring>
#include <exception>
#include <limits>
#include <numeric>
#include <stdexcept>
#include <type_traits>
#include "predicates.h"

static constexpr sym2::Blob::Data2 preZero = {.name = {'\0'}};
static constexpr sym2::Blob::Data4 midZero = {.name = {'\0'}};
static constexpr sym2::Blob::Data8 mainZero = {.name = {'\0'}};
static constexpr std::size_t smallNameLength = sizeof(sym2::Blob::Data2::name) + sizeof(sym2::Blob::Data4::name) - 1;
static constexpr std::size_t largeNameLength = smallNameLength + sizeof(sym2::Blob::Data8::name);

namespace sym2 {
    namespace {
        struct ChildBlobNumberGuard {
            /* RAII class to make sure we don't forget to update the number of child blobs at the end of a ctor body.
             * Shall guard against early returns. Exceptions aren't an issue (then the object has no lifetime anyhow).
             * Instances should be declared at the beginning of a ctor, non-statically. */
            SmallVecBase<Blob>& buffer;
            const std::size_t index;

            ~ChildBlobNumberGuard()
            {
                if (std::uncaught_exceptions() != 0)
                    /* Make sure the assert below doesn't fire if stack unwinding is in process anyhow. */
                    return;

                assert(buffer.size() > 1);

                buffer[index].main.nChildBlobs = buffer.size() - index - 1;
            }
        };
    }
}

sym2::Expr::Expr(std::int32_t n)
{
    appendSmallInt(n);
}

sym2::Expr::Expr(double n)
    : buffer{Blob{.header = Type::floatingPoint,
      .flags = Flag::numericallyEvaluable,
      .pre = preZero,
      .mid = midZero,
      .main = {.inexact = n}}}
{}

sym2::Expr::Expr(std::int32_t num, std::int32_t denom)
{
    if (denom == 0)
        throw std::domain_error("Zero denominator of small exact rational");

    if (denom < 0) {
        denom = -denom;
        num = -num;
    }

    appendSmallRationalOrInt(num, denom);
}

sym2::Expr::Expr(const LargeInt& n)
{
    appendSmallOrLargeInt(n);
}

sym2::Expr::Expr(const LargeRational& n)
{
    const auto num = numerator(n);
    const auto denom = denominator(n);

    if (fitsInto<std::int32_t>(num) && fitsInto<std::int32_t>(denom)) {
        appendSmallRationalOrInt(static_cast<std::int32_t>(num), static_cast<std::int32_t>(denom));
        return;
    }

    const ChildBlobNumberGuard childBlobNumberUpdater{buffer, 0};

    buffer.push_back(Blob{.header = Type::largeRational,
      .flags = Flag::numericallyEvaluable,
      .pre = preZero,
      .mid = {.nLogicalOrPhysicalChildren = 2},
      .main = mainZero /* Number of child blobs to be determined. */});

    appendSmallOrLargeInt(num);
    appendSmallOrLargeInt(denom);
}

sym2::Expr::Expr(std::string_view symbol)
    : buffer{Blob{.header = Type::symbol, .flags = Flag::none, .pre = preZero, .mid = midZero, .main = mainZero}}
{
    copyNameOrThrow(symbol, largeNameLength);
}

sym2::Expr::Expr(std::string_view symbol, SymbolFlag constraint)
    : Expr{symbol}
{
    Flag& flags = buffer.front().flags;

    switch (constraint) {
        case SymbolFlag::real:
            flags |= Flag::real;
            break;
        case SymbolFlag::positive:
            flags |= Flag::positive;
            break;
        case SymbolFlag::positiveReal:
            flags |= Flag::real | Flag::positive;
            break;
    }
}

sym2::Expr::Expr(std::string_view constant, double value)
    : buffer{Blob{.header = Type::constant,
      .flags = Flag::numericallyEvaluable,
      .pre = preZero,
      .mid = midZero,
      .main = {.inexact = value}}}
{
    copyNameOrThrow(constant, smallNameLength);
}

sym2::Expr::Expr(std::string_view function, ExprView<> arg, UnaryDoubleFctPtr eval)
    : buffer{Blob{.header = Type::function,
      .flags = Flag::none /* TODO */,
      .pre = preZero,
      .mid = {.nLogicalOrPhysicalChildren = 1},
      .main = mainZero}}
{
    const ChildBlobNumberGuard childBlobNumberUpdater{buffer, 0};

    buffer.push_back(Blob{
      .header = Type::functionId, .flags = Flag::none, .pre = preZero, .mid = midZero, .main = {.unaryEval = eval}});

    copyNameOrThrow(function, smallNameLength, 1);

    buffer.reserve(arg.size() + 2);

    std::copy(arg.begin(), arg.end(), std::back_inserter(buffer));
}

sym2::Expr::Expr(std::string_view function, ExprView<> arg1, ExprView<> arg2, BinaryDoubleFctPtr eval)
    : buffer{Blob{.header = Type::function,
      .flags = Flag::none /* TODO */,
      .pre = preZero,
      .mid = {.nLogicalOrPhysicalChildren = 2},
      .main = mainZero}}
{
    const ChildBlobNumberGuard childBlobNumberUpdater{buffer, 0};

    buffer.push_back(Blob{
      .header = Type::functionId, .flags = Flag::none, .pre = preZero, .mid = midZero, .main = {.binaryEval = eval}});

    copyNameOrThrow(function, smallNameLength, 1);

    buffer.reserve(arg1.size() + arg2.size() + 2);

    for (ExprView<> arg : {arg1, arg2})
        std::copy(arg.begin(), arg.end(), std::back_inserter(buffer));
}

sym2::Expr::Expr(Type composite, std::span<const Expr> ops)
    : Expr{composite, ops.size()}
{
    appendOperands(composite, ops);
}

sym2::Expr::Expr(Type composite, std::span<const ExprView<>> ops)
    : Expr{composite, ops.size()}
{
    appendOperands(composite, ops);
}

sym2::Expr::Expr(Type composite, std::initializer_list<ExprView<>> ops)
    : Expr{composite, ops.size()}
{
    appendOperands(composite, ops);
}

sym2::Expr::Expr(Type composite, std::size_t nOps)
    : buffer{Blob{.header = composite,
      .flags = Flag::none,
      .pre = preZero,
      .mid = {.nLogicalOrPhysicalChildren = static_cast<std::uint32_t>(nOps)},
      .main = mainZero /* Number of child blobs to be determined. */}}
{}

template <class Range>
void sym2::Expr::appendOperands(Type composite, const Range& ops)
{
    const ChildBlobNumberGuard childBlobNumberUpdater{buffer, 0};

    assert(ops.size() <= std::numeric_limits<std::uint32_t>::max());
    assert(composite == Type::sum || composite == Type::product || composite == Type::power
      || composite == Type::complexNumber);

    if (composite == Type::complexNumber
      && (ops.size() != 2 || !std::all_of(ops.begin(), ops.end(), is < number && realDomain >)))
        throw std::invalid_argument("Complex numbers must be created with two non-complex arguments");
    else if (composite == Type::power && ops.size() != 2)
        throw std::invalid_argument("Powers must be created with exactly two operands");

    /* Likely to be more, but we also don't premature allocation if it might just fit in-place: */
    buffer.reserve(ops.size());

    bool numEval = true;

    for (ExprView<> ev : ops) {
        numEval = numEval && is<numericallyEvaluable>(ev);
        buffer.insert(buffer.end(), ev.begin(), ev.end());
    }

    if (numEval)
        buffer.front().flags = Flag::numericallyEvaluable;
}

void sym2::Expr::appendSmallInt(std::int32_t n)
{
    buffer.push_back(Blob{.header = Type::smallInt,
      .flags = Flag::numericallyEvaluable,
      .pre = preZero,
      .mid = midZero,
      .main = {.exact = {n, 1}}});
}

void sym2::Expr::appendSmallRationalOrInt(std::int32_t num, std::int32_t denom)
{
    const auto divisor = std::gcd(num, denom);

    num /= divisor;
    denom /= divisor;

    if (denom == 1)
        appendSmallInt(num);
    else
        buffer.push_back(Blob{.header = Type::smallRational,
          .flags = Flag::numericallyEvaluable,
          .pre = preZero,
          .mid = midZero,
          .main = {.exact = {num, denom}}});
}

void sym2::Expr::appendSmallOrLargeInt(const LargeInt& n)
{
    if (fitsInto<std::int32_t>(n))
        appendSmallInt(static_cast<std::int32_t>(n));
    else
        appendLargeInt(n);
}

void sym2::Expr::appendLargeInt(const LargeInt& n)
{
    static constexpr auto opSize = sizeof(Blob);

    buffer.push_back(Blob{.header = Type::largeInt,
      .flags = Flag::numericallyEvaluable,
      .pre = preZero,
      .mid = {.largeIntSign = n < 0 ? -1 : 1},
      .main = mainZero /* Number of child blobs to be determined. */});

    /* Save the index instead of a reference to back(), which might be invalidated below. */
    const std::size_t frontIdx = buffer.size() - 1;
    const ChildBlobNumberGuard childBlobNumberUpdater{buffer, frontIdx};

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

    /* Rotate any trailing zero bytes to the front. When import and exports of large integer bits happens with the
     * most significant bits first, leading zeros are dropped. This allows for an easier import, as the whole
     * Blob span can be used as the bit source: */
    std::rotate(aliased, aliased + length * opSize - (opSize - remainder), aliased + length * opSize);
}

void sym2::Expr::copyNameOrThrow(std::string_view name, std::uint8_t maxLength, std::size_t bufferIndex)
{
    assert(buffer.size() >= bufferIndex + 1);

    auto* dest = std::next(reinterpret_cast<char*>(&buffer[bufferIndex]), 2);

    if (name.length() > maxLength || name.empty())
        throw std::invalid_argument("Names must be non-empty and < 13 characters long");

    std::copy(name.cbegin(), name.cend(), dest);
}

sym2::Expr::operator sym2::ExprView<>() const
{
    assert(buffer.size() >= 1);

    return ExprView<>{buffer.data(), buffer.size()};
}

sym2::Expr sym2::operator"" _ex(const char* str, std::size_t)
{
    return Expr{str};
}

sym2::Expr sym2::operator"" _ex(unsigned long long n)
{
    if (n > std::numeric_limits<std::int32_t>::max())
        throw std::domain_error("Integral Expr literals must fit into an 32bit int");

    return Expr{static_cast<std::int32_t>(n)};
}

sym2::Expr sym2::operator"" _ex(long double n)
{
    return Expr{static_cast<double>(n)};
}
