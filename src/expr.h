#pragma once

#include <array>
#include <boost/rational.hpp>
#include <cstdint>
#include <initializer_list>
#include <span>
#include <string_view>
#include "rational.h"
#include "smallvec.h"

namespace sym2 {
    enum class Type : std::uint8_t {
        symbol,
        constant,
        smallInt,
        smallRational,
        largeInt,
        /* A large rational can still have one small/inplace integer (either numerator or denominator): */
        largeRational,
        floatingPoint,
        complexNumber,
        sum,
        product,
        power,
        function
    };

    enum class Sign : std::uint8_t { unknown, positive, negative, neither };
    enum class Flag : std::uint8_t { none, numericallyEvaluable };

    Flag operator|(Flag lhs, Flag rhs);
    Flag& operator|=(Flag& lhs, Flag rhs);
    Flag operator&(Flag lhs, Flag rhs);
    Flag& operator&=(Flag& lhs, Flag rhs);

    struct SmallRational {
        std::int32_t num;
        std::int32_t denom;
    };

    struct Operand {
        Type header;
        Sign sign : 2;
        Flag flags : 6;
        /* Use the rest to allow for longer variable names: */
        char name[6];

        union {
            std::size_t count;
            char name[8];
            SmallRational exact;
            double inexact;
            std::uint64_t limb;
        } data;
    };

    using ExprView = std::span<const Operand>;

    bool operator==(ExprView lhs, ExprView rhs);
    bool operator!=(ExprView lhs, ExprView rhs);

    class Expr {
      public:
        Expr(int n);
        Expr(double n);
        Expr(int num, int denom);
        explicit Expr(const Int& n);
        explicit Expr(const Rational& n);
        explicit Expr(std::string_view symbol);
        /* Construct a constant: */
        explicit Expr(std::string_view constant, double value);
        explicit Expr(ExprView e);
        Expr(Type composite, std::span<const ExprView> ops);
        Expr(Type composite, std::initializer_list<ExprView> ops);

        operator ExprView() const;

      private:
        void appendSmallInt(std::int32_t n);
        void appendFloatingPoint(double n);
        void appendSmallRationalOrInt(std::int32_t num, std::int32_t denom);
        void appendSmallOrLargeInt(const Int& n);
        void appendLargeInt(const Int& n);

        SmallVec<Operand, 10> buffer;
    };

    Expr operator"" _ex(const char* str, std::size_t);
    Expr operator"" _ex(unsigned long long n);
}
