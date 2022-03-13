#pragma once

#include <cstdint>
#include <initializer_list>
#include <memory_resource>
#include <span>
#include <string_view>
#include <vector>
#include "doublefctptr.h"
#include "exprliteral.h"
#include "largerational.h"
#include "sym2/symbolflag.h"
#include "view.h"

namespace sym2 {
    struct Blob;
    struct LargeRationalRef {
        // We need this little constructor helper for disabling Boost Multiprecision's attempt to
        // turn an ExprView into a large int/rational (because it's a range - while the check
        // correctly results in the inability to construct the large int/rational, it needs to know
        // what a Blob is; but we want to hide its definition, so this can't compile).
        const LargeRational& value;
    };
    struct LargeIntRef {
        // See LargeRationalRef.
        const LargeInt& value;
    };

    enum class CompositeType { sum, product, power, complexNumber };

    class Expr {
      public:
        using allocator_type = std::pmr::polymorphic_allocator<>;

        Expr(std::int32_t n, allocator_type allocator = {});
        Expr(double n, allocator_type allocator = {});
        Expr(std::int32_t num, std::int32_t denom, allocator_type allocator = {});
        explicit Expr(LargeIntRef n, allocator_type allocator = {});
        explicit Expr(LargeRationalRef n, allocator_type allocator = {});
        explicit Expr(std::string_view symbol, allocator_type allocator = {});
        Expr(std::string_view symbol, SymbolFlag constraint, allocator_type allocator = {});
        Expr(std::string_view constant, double value,
          allocator_type allocator = {}); // Constructs a constant
        Expr(std::string_view function, ExprView<> arg, UnaryDoubleFctPtr eval,
          allocator_type allocator = {});
        Expr(std::string_view function, ExprView<> arg1, ExprView<> arg2, BinaryDoubleFctPtr eval,
          allocator_type allocator = {});
        explicit Expr(ExprView<> e, allocator_type allocator = {});
        Expr(CompositeType composite, std::span<const ExprView<>> ops, allocator_type allocator);
        Expr(CompositeType composite, std::span<const Expr> ops, allocator_type allocator);
        Expr(
          CompositeType composite, std::initializer_list<ExprView<>> ops, allocator_type allocator);
        Expr(ExprLiteral literal, allocator_type allocator);

        // All = defaulted in the TU, which is required due to the shielded Blob definition:
        Expr(const Expr& other, allocator_type allocator = {});
        Expr& operator=(const Expr& other);
        Expr(Expr&& other, allocator_type allocator); // Can't be noexcept when allocators differ
        Expr& operator=(Expr&& other); // Can't be noexcept when allocators differ
        ~Expr();

        // Implicit conversions to any tag are indeed desired here:
        template <PredicateTag auto tag>
        operator ExprView<tag>() const
        {
            return ExprView<tag>{view()};
        }

      private:
        // Used internally to avoid duplication.
        explicit Expr(CompositeType composite, std::size_t nOps, allocator_type allocator = {});

        void appendSmallInt(std::int32_t n);
        void appendSmallRationalOrInt(std::int32_t num, std::int32_t denom);
        void appendSmallOrLargeInt(const LargeInt& n);
        void appendLargeInt(const LargeInt& n);
        void copyNameOrThrow(
          std::string_view name, std::uint8_t maxLength, std::size_t bufferIndex = 0);
        template <class Range>
        void appendOperands(CompositeType composite, const Range& ops);

        ExprView<> view() const;

        std::pmr::vector<Blob> buffer;
    };
}
