#pragma once

#include <cstdint>
#include <initializer_list>
#include <memory_resource>
#include <span>
#include <string_view>
#include <vector>
#include "blob.h"
#include "largerational.h"
#include "sym2/symbolflag.h"
#include "view.h"

namespace sym2 {
    class Expr {
      public:
        Expr(std::int32_t n);
        Expr(double n);
        Expr(std::int32_t num, std::int32_t denom);
        explicit Expr(const LargeInt& n);
        explicit Expr(const LargeRational& n);
        explicit Expr(std::string_view symbol);
        Expr(std::string_view symbol, SymbolFlag constraint);
        /* Construct a constant: */
        Expr(std::string_view constant, double value);
        Expr(std::string_view function, ExprView<> arg, UnaryDoubleFctPtr eval);
        Expr(std::string_view function, ExprView<> arg1, ExprView<> arg2, BinaryDoubleFctPtr eval);
        template <auto tag>
        explicit Expr(ExprView<tag> e)
            : buffer{e.begin(), e.end()}
        {}
        Expr(Type composite, std::span<const ExprView<>> ops);
        Expr(Type composite, std::span<const Expr> ops);
        Expr(Type composite, std::initializer_list<ExprView<>> ops);

        /* Implicit conversions to any tag are indeed desired here: */
        template <PredicateTag auto tag>
        operator ExprView<tag>() const
        {
            return ExprView<tag>{buffer.data(), buffer.size()};
        }

      private:
        /* Used internally to avoid duplication. */
        explicit Expr(Type composite, std::size_t nOps);

        void appendSmallInt(std::int32_t n);
        void appendSmallRationalOrInt(std::int32_t num, std::int32_t denom);
        void appendSmallOrLargeInt(const LargeInt& n);
        void appendLargeInt(const LargeInt& n);
        void copyNameOrThrow(std::string_view name, std::uint8_t maxLength, std::size_t bufferIndex = 0);
        template <class Range>
        void appendOperands(Type composite, const Range& ops);

        std::pmr::vector<Blob> buffer;
    };

    Expr operator"" _ex(const char* str, std::size_t);
    Expr operator"" _ex(unsigned long long n);
    Expr operator"" _ex(long double);
}
