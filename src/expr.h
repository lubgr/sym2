#pragma once

#include <cstdint>
#include <initializer_list>
#include <span>
#include <string_view>
#include "blob.h"
#include "largerational.h"
#include "smallvec.h"
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
        /* Construct a constant: */
        Expr(std::string_view constant, double value);
        Expr(std::string_view function, ExprView arg, UnaryDoubleFctPtr eval);
        Expr(std::string_view function, ExprView arg1, ExprView arg2, BinaryDoubleFctPtr eval);
        explicit Expr(ExprView e);
        Expr(Type composite, std::span<const ExprView> ops);
        Expr(Type composite, std::initializer_list<ExprView> ops);

        operator ExprView() const;

      private:
        void appendSmallInt(std::int32_t n);
        void appendSmallRationalOrInt(std::int32_t num, std::int32_t denom);
        void appendSmallOrLargeInt(const LargeInt& n);
        void appendLargeInt(const LargeInt& n);
        void copyNameOrThrow(std::string_view name, std::uint8_t maxLength, std::size_t bufferIndex = 0);

        SmallVec<Blob, 10> buffer;
    };

    Expr operator"" _ex(const char* str, std::size_t);
    Expr operator"" _ex(unsigned long long n);
}
