#pragma once

#include <cstdint>
#include <initializer_list>
#include <memory_resource>
#include <span>
#include <string_view>
#include <vector>
#include "doublefctptr.h"
#include "largerational.h"
#include "sym2/symbolflag.h"
#include "view.h"

namespace sym2 {
    struct Blob;
    struct LargeRationalRef {
        // We need this little constructor helper for disabling Boost Multiprecision's attempt to turn an ExprView into
        // a large int/rational (because it's a range - while the check correctly results in the inability to construct
        // the large int/rational, it needs to know what a Blob is; but we want to hide its definition, so this can't
        // compile).
        const LargeRational& value;
    };
    struct LargeIntRef {
        // See LargeRationalRef.
        const LargeInt& value;
    };

    enum class CompositeType { sum, product, power, complexNumber };

    class Expr {
      public:
        Expr(std::int32_t n);
        Expr(double n);
        Expr(std::int32_t num, std::int32_t denom);
        explicit Expr(LargeIntRef n);
        explicit Expr(LargeRationalRef n);
        explicit Expr(std::string_view symbol);
        Expr(std::string_view symbol, SymbolFlag constraint);
        Expr(std::string_view constant, double value); // Constructs a constant
        Expr(std::string_view function, ExprView<> arg, UnaryDoubleFctPtr eval);
        Expr(std::string_view function, ExprView<> arg1, ExprView<> arg2, BinaryDoubleFctPtr eval);
        explicit Expr(ExprView<> e);
        Expr(CompositeType composite, std::span<const ExprView<>> ops);
        Expr(CompositeType composite, std::span<const Expr> ops);
        Expr(CompositeType composite, std::initializer_list<ExprView<>> ops);

        /* All = defaulted in the TU, which is required due to the shielded Blob definition: */
        Expr(const Expr& other);
        Expr& operator=(const Expr& other);
        Expr(Expr&& other) noexcept;
        Expr& operator=(Expr&& other) noexcept;
        ~Expr();

        /* Implicit conversions to any tag are indeed desired here: */
        template <PredicateTag auto tag>
        operator ExprView<tag>() const
        {
            return ExprView<tag>{view()};
        }

      private:
        /* Used internally to avoid duplication. */
        explicit Expr(CompositeType composite, std::size_t nOps);

        void appendSmallInt(std::int32_t n);
        void appendSmallRationalOrInt(std::int32_t num, std::int32_t denom);
        void appendSmallOrLargeInt(const LargeInt& n);
        void appendLargeInt(const LargeInt& n);
        void copyNameOrThrow(std::string_view name, std::uint8_t maxLength, std::size_t bufferIndex = 0);
        template <class Range>
        void appendOperands(CompositeType composite, const Range& ops);

        ExprView<> view() const;

        std::pmr::vector<Blob> buffer;
    };

    Expr operator"" _ex(const char* str, std::size_t);
    Expr operator"" _ex(unsigned long long n);
    Expr operator"" _ex(long double);
}
