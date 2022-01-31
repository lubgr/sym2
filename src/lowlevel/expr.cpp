
#include "expr.h"
#include <algorithm>
#include <boost/iterator/function_output_iterator.hpp>
#include <boost/range/algorithm.hpp>
#include <cassert>
#include <cstring>
#include <exception>
#include <limits>
#include <numeric>
#include <stdexcept>
#include <type_traits>
#include "blob.h"
#include "predicates.h"

static constexpr sym2::Blob::Data2 preZero = {.name = {'\0'}};
static constexpr sym2::Blob::Data4 midZero = {.name = {'\0'}};
static constexpr sym2::Blob::Data8 mainZero = {.name = {'\0'}};
static constexpr std::size_t smallNameLength =
  sizeof(sym2::Blob::Data2::name) + sizeof(sym2::Blob::Data4::name) - 1;
static constexpr std::size_t largeNameLength = smallNameLength + sizeof(sym2::Blob::Data8::name);

namespace sym2 {
    struct ChildBlobNumberGuard {
        /* RAII class to make sure we don't forget to update the number of child blobs at the end of
         * a ctor body. Shall guard against early returns. Exceptions aren't an issue (then the
         * object has no lifetime anyhow).
         * Instances should be declared at the beginning of a ctor, non-statically. */
        std::pmr::vector<Blob>& buffer;
        const std::size_t index;

        ~ChildBlobNumberGuard()
        {
            if (std::uncaught_exceptions() != 0)
                /* Make sure the assert below doesn't fire if stack unwinding is in process anyhow.
                 */
                return;

            assert(buffer.size() > 1);

            buffer[index].main.nChildBlobs = buffer.size() - index - 1;
        }
    };

    namespace {
        Type toInternalType(CompositeType composite)
        {
            switch (composite) {
                case CompositeType::sum:
                    return Type::sum;
                case CompositeType::product:
                    return Type::product;
                case CompositeType::power:
                    return Type::power;
                case CompositeType::complexNumber:
                    return Type::complexNumber;
                default:
                    assert(false && "Unhandled composite type");
                    return Type::sum; // Random choice
            }
        }
    }
}

sym2::Expr::Expr(std::int32_t n, allocator_type allocator)
    : buffer{allocator}
{
    appendSmallInt(n);
}

sym2::Expr::Expr(double n, allocator_type allocator)
    : buffer{{floatingPointBlob(n)}, allocator}
{
    if (!std::isfinite(n))
        throw std::domain_error("Floating point Expr must be of finite value");
}

sym2::Expr::Expr(std::int32_t num, std::int32_t denom, allocator_type allocator)
    : buffer{allocator}
{
    if (denom == 0)
        throw std::domain_error("Zero denominator of small exact rational");

    if (denom < 0) {
        denom = -denom;
        num = -num;
    }

    appendSmallRationalOrInt(num, denom);
}

sym2::Expr::Expr(LargeIntRef n, allocator_type allocator)
    : buffer{allocator}
{
    appendSmallOrLargeInt(n.value);
}

sym2::Expr::Expr(LargeRationalRef n, allocator_type allocator)
    : buffer{allocator}
{
    const auto num = numerator(n.value);
    const auto denom = denominator(n.value);

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

sym2::Expr::Expr(std::string_view symbol, allocator_type allocator)
    : buffer{{symbolBlob(symbol)}, allocator}
{}

sym2::Expr::Expr(std::string_view symbol, SymbolFlag constraint, allocator_type allocator)
    : Expr{symbol, allocator}
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

sym2::Expr::Expr(std::string_view constant, double value, allocator_type allocator)
    : buffer{{Blob{.header = Type::constant,
               .flags = Flag::numericallyEvaluable,
               .pre = preZero,
               .mid = midZero,
               .main = {.inexact = value}}},
      allocator}
{
    copyNameOrThrow(constant, smallNameLength);
}

sym2::Expr::Expr(
  std::string_view function, ExprView<> arg, UnaryDoubleFctPtr eval, allocator_type allocator)
    : buffer{{Blob{.header = Type::function,
               .flags = Flag::none /* TODO */,
               .pre = preZero,
               .mid = {.nLogicalOrPhysicalChildren = 1},
               .main = mainZero}},
      allocator}
{
    const ChildBlobNumberGuard childBlobNumberUpdater{buffer, 0};

    buffer.push_back(Blob{.header = Type::functionId,
      .flags = Flag::none,
      .pre = preZero,
      .mid = midZero,
      .main = {.unaryEval = eval}});

    copyNameOrThrow(function, smallNameLength, 1);

    buffer.reserve(arg.size() + 2);

    std::copy(arg.begin(), arg.end(), std::back_inserter(buffer));
}

sym2::Expr::Expr(std::string_view function, ExprView<> arg1, ExprView<> arg2,
  BinaryDoubleFctPtr eval, allocator_type allocator)
    : buffer{{Blob{.header = Type::function,
               .flags = Flag::none /* TODO */,
               .pre = preZero,
               .mid = {.nLogicalOrPhysicalChildren = 2},
               .main = mainZero}},
      allocator}
{
    const ChildBlobNumberGuard childBlobNumberUpdater{buffer, 0};

    buffer.push_back(Blob{.header = Type::functionId,
      .flags = Flag::none,
      .pre = preZero,
      .mid = midZero,
      .main = {.binaryEval = eval}});

    copyNameOrThrow(function, smallNameLength, 1);

    buffer.reserve(arg1.size() + arg2.size() + 2);

    for (ExprView<> arg : {arg1, arg2})
        std::copy(arg.begin(), arg.end(), std::back_inserter(buffer));
}

sym2::Expr::Expr(ExprView<> e, allocator_type allocator)
    : buffer{e.begin(), e.end(), allocator}
{}

sym2::Expr::Expr(CompositeType composite, std::span<const Expr> ops, allocator_type allocator)
    : Expr{composite, ops.size(), allocator}
{
    appendOperands(composite, ops);
}

sym2::Expr::Expr(CompositeType composite, std::span<const ExprView<>> ops, allocator_type allocator)
    : Expr{composite, ops.size(), allocator}
{
    appendOperands(composite, ops);
}

sym2::Expr::Expr(
  CompositeType composite, std::initializer_list<ExprView<>> ops, allocator_type allocator)
    : Expr{composite, ops.size(), allocator}
{
    appendOperands(composite, ops);
}

sym2::Expr::Expr(ExprLiteral literal, allocator_type allocator)
    : Expr{static_cast<ExprView<>>(literal), allocator}
{}

sym2::Expr::Expr(CompositeType composite, std::size_t nOps, allocator_type allocator)
    : buffer{{Blob{.header = toInternalType(composite),
               .flags = Flag::none,
               .pre = preZero,
               .mid = {.nLogicalOrPhysicalChildren = static_cast<std::uint32_t>(nOps)},
               .main = mainZero /* Number of child blobs to be determined. */}},
      allocator}
{}

template <class Range>
void sym2::Expr::appendOperands(CompositeType composite, const Range& ops)
{
    const ChildBlobNumberGuard childBlobNumberUpdater{buffer, 0};

    assert(ops.size() <= std::numeric_limits<std::uint32_t>::max());

    if (composite == CompositeType::complexNumber
      && (ops.size() != 2 || !std::all_of(ops.begin(), ops.end(), is < number && realDomain >)))
        throw std::invalid_argument(
          "Complex numbers must be created with two numeric real-valued arguments");
    else if (composite == CompositeType::power && ops.size() != 2)
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
    buffer.push_back(smallIntBlob(n));
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

    /* Rotate any trailing zero bytes to the front. When import and exports of large integer bits
     * happens with the most significant bits first, leading zeros are dropped. This allows for an
     * easier import, as the whole Blob span can be used as the bit source: */
    std::rotate(
      aliased, aliased + length * opSize - (opSize - remainder), aliased + length * opSize);
}

void sym2::Expr::copyNameOrThrow(
  std::string_view name, std::uint8_t maxLength, std::size_t bufferIndex)
{
    assert(buffer.size() >= bufferIndex + 1);

    auto* dest = std::next(reinterpret_cast<char*>(&buffer[bufferIndex]), 2);

    if (name.length() > maxLength || name.empty())
        throw std::invalid_argument("Names must be non-empty not exceed a tight length limit");

    std::copy(name.cbegin(), name.cend(), dest);
}

sym2::Expr::Expr(const Expr& other, allocator_type allocator)
    : buffer{other.buffer, allocator}
{}

sym2::Expr& sym2::Expr::operator=(const Expr& other) = default;

sym2::Expr::Expr(Expr&& other, allocator_type allocator)
    : buffer{std::move(other.buffer), allocator}
{}

sym2::Expr& sym2::Expr::operator=(Expr&& other) = default;

sym2::Expr::~Expr() = default;

sym2::ExprView<> sym2::Expr::view() const
{
    return ExprView<>{buffer.data(), buffer.size()};
}
