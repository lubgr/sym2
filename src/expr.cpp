
#include "sym2/expr.h"
#include <algorithm>
#include <cassert>
#include <cstring>
#include <exception>
#include <limits>
#include <numeric>
#include <stdexcept>
#include <type_traits>
#include "blob.h"
#include "sym2/predicates.h"

sym2::Expr::Expr(allocator_type allocator)
    : Expr{std::int16_t{0}, allocator}
{}

sym2::Expr::Expr(std::int16_t n, allocator_type allocator)
    : buffer{{construct(n)}, allocator}
{}

sym2::Expr::Expr(std::int32_t n, allocator_type allocator)
    : Expr{static_cast<std::int16_t>(n), allocator}
{
    if (n > std::numeric_limits<std::int16_t>::max())
        throw std::domain_error("Small integer expressions must fit into 16 bits");
}

sym2::Expr::Expr(std::int16_t num, std::int16_t denom, allocator_type allocator)
    : buffer{{[&]() -> Blob {
                 if (denom == 0)
                     throw std::invalid_argument{
                       "Zero denominator during small rational construction"};
                 return construct(num, denom);
             }()},
      allocator}
{}

sym2::Expr::Expr(std::int32_t num, std::int32_t denom, allocator_type allocator)
    : Expr{static_cast<std::int16_t>(num), static_cast<std::int16_t>(denom), allocator}
{
    if (num > std::numeric_limits<std::int16_t>::max()
      || denom > std::numeric_limits<std::int16_t>::max())
        throw std::domain_error(
          "Numerator and denominator of small rationals must fit into 16 bits");
}

sym2::Expr::Expr(double n, allocator_type allocator)
    : buffer{constructSequence(n, allocator)}
{
    if (!std::isfinite(n))
        throw std::domain_error("Floating point value must be finite");
}

sym2::Expr::Expr(const LargeInt& n, allocator_type allocator)
    : buffer{[=]() -> LocalVec<Blob> {
                 if (fitsInto<std::int16_t>(n))
                     return {{construct(static_cast<std::int16_t>(n))}, allocator};
                 else
                     return constructSequence(n, allocator);
             }(),
      allocator}
{}

sym2::Expr::Expr(const LargeRational& n, allocator_type allocator)
    : buffer{[=]() -> LocalVec<Blob> {
        const auto num = numerator(n);
        const auto denom = denominator(n);

        if (fitsInto<std::int16_t>(num) && fitsInto<std::int16_t>(denom))
            return {{construct(static_cast<std::int16_t>(num), static_cast<std::int16_t>(denom))},
              allocator};

        return constructSequence(n, allocator);
    }()}
{}

sym2::Expr::Expr(std::string_view symbol, allocator_type allocator)
    : Expr{symbol, DomainFlag::none, allocator}
{}

sym2::Expr::Expr(std::string_view symbol, DomainFlag domain, allocator_type allocator)
    : buffer{[=]() -> LocalVec<Blob> {
        if (isSmallName(symbol))
            return {{construct(symbol, domain)}, allocator};
        else
            return constructSequence(symbol, domain, allocator);
    }()}
{
    if (symbol.size() == 0)
        throw std::invalid_argument{"Empty symbol names are invalid"};
}

sym2::Expr::Expr(std::string_view constant, double value, allocator_type allocator)
    : buffer{[=]() -> LocalVec<Blob> {
        if (constant.empty())
            throw std::invalid_argument{"Constant name must be non-empty"};
        if (!std::isfinite(value))
            throw std::domain_error("Constant floating point value must be finite");

        return constructSequence(constant, value, allocator);
    }()}
{}

sym2::Expr::Expr(
  std::string_view function, ExprView<> arg, UnaryDoubleFctPtr eval, allocator_type allocator)
    : buffer{constructSequence(function, arg.get(), eval, allocator)}
{}

sym2::Expr::Expr(std::string_view function, ExprView<> arg1, ExprView<> arg2,
  BinaryDoubleFctPtr eval, allocator_type allocator)
    : buffer{constructSequence(function, arg1.get(), arg2.get(), eval, allocator)}
{}

sym2::Expr::Expr(ExprView<> e, allocator_type allocator)
    : buffer{constructDuplicateSequence(e.get(), allocator)}
{}

namespace sym2 {
    namespace {
        template <class T, class BlobRetrieveFct>
        void constructComposite(CompositeType composite, const std::span<const T> ops,
          LocalVec<Blob>& buffer, BlobRetrieveFct&& get)
        {
            if (composite == CompositeType::complexNumber
              && (ops.size() != 2
                || !std::all_of(ops.begin(), ops.end(), is < number && realDomain >)))
                throw std::invalid_argument(
                  "Complex numbers must be created with two numeric real-valued arguments");
            else if (composite == CompositeType::power && ops.size() != 2)
                throw std::invalid_argument("Powers must be created with exactly two operands");

            const std::uint32_t totalExtent =
              std::transform_reduce(ops.begin(), ops.end(), std::uint32_t{0}, std::plus<>{},
                [](const ExprView<> e) { return remoteExtent(e.get()) + 1; });
            const auto numOperands = static_cast<std::uint16_t>(ops.size());

            if (totalExtent > std::numeric_limits<std::uint16_t>::max())
                throw std::range_error{"Can't handle composite expression of given size"};

            buffer.reserve(totalExtent + 1);
            // We only resize to hold all immediate logical operands of the composite expression,
            // since we use the size() below to know where sub-expression data of the immediate
            // operands should be copied.
            buffer.resize(numOperands + 1, Blob{});

            buffer[0] = constructCompositeHeader(composite, numOperands, totalExtent);

            for (std::uint16_t i = 0; i < numOperands; ++i) {
                const Blob* const src = get(ops[i]);

                appendDuplicateSequence(src, i + 1, buffer);
            }
        }
    }
}

sym2::Expr::Expr(CompositeType composite, std::span<const Expr> ops, allocator_type allocator)
    : buffer{allocator}
{
    constructComposite(composite, ops, buffer, [](const Expr& op) { return op.buffer.data(); });
}

sym2::Expr::Expr(CompositeType composite, std::span<const ExprView<>> ops, allocator_type allocator)
    : buffer{allocator}
{
    constructComposite(composite, ops, buffer, [](const ExprView<> op) { return op.get(); });
}

sym2::Expr::Expr(CompositeType composite, ExprView<> op1, ExprView<> op2, allocator_type allocator)
    : Expr{composite, {{op1, op2}}, allocator}
{}

sym2::Expr::Expr(const Expr& other, allocator_type allocator)
    : buffer{other.buffer, allocator}
{}

sym2::Expr::Expr(Expr&& other, allocator_type allocator)
    : buffer{std::move(other.buffer), allocator}
{}

sym2::FixedExpr<2> sym2::operator"" _ex(const long double n)
{
    return FixedExpr<2>{static_cast<double>(n)};
}

sym2::FixedExpr<1> sym2::operator"" _ex(const char* str, const std::size_t length)
{
    return FixedExpr<1>{std::string_view{str, length}};
}

sym2::FixedExpr<1> sym2::operator"" _ex(const unsigned long long n)
{
    if (n > static_cast<unsigned long long>(std::numeric_limits<std::int16_t>::max()))
        throw std::domain_error("Small integers must fit into 16 bits");

    return FixedExpr<1>{static_cast<std::int16_t>(n)};
}
