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
    enum class Flag : std::uint8_t {
        symbol,
        constant,
        smallInt,
        smallRational,
        floatingPoint,
        largeRational,
        complexNumber,
        sum,
        product,
        power,
        function
    };

    struct SmallRational {
        std::int32_t num;
        std::int32_t denom;
    };

    struct Operand {
        Flag header = Flag::smallInt;
        bool hasLargeRationals = false;
        /* Use the rest to allow for longer variable names: */
        char name[6] = {'\0'};

        union {
            std::size_t count;
            char name[8];
            SmallRational exact;
            double inexact;
            Rational* large;
        } data{0};
    };

    using ExprView = std::span<const Operand>;

    bool operator==(ExprView lhs, ExprView rhs);
    bool operator!=(ExprView lhs, ExprView rhs);

    class Expr {
      public:
        Expr(int n);
        Expr(double n);
        Expr(int num, int denom);
        explicit Expr(const Rational& n);
        explicit Expr(Rational&& n);
        explicit Expr(std::string_view symbol);
        explicit Expr(ExprView e);
        Expr(Flag composite, std::span<const ExprView> ops);
        Expr(Flag composite, std::initializer_list<ExprView> ops);
        Expr(const Expr& other);
        Expr& operator=(Expr other);
        Expr(Expr&& other) = default;
        Expr& operator=(Expr&& other) = default;
        ~Expr() = default;

        operator ExprView() const;

        friend void swap(Expr& lhs, Expr& rhs);

      private:
        void storeAndUpdateRationals();
        void updateRationalPointer();

        SmallVec<Operand, 10> small;
        /* Importante note: when changing this to a vector with a local buffer, we need to implemented move (assignment)
         * constructors that are currently = defaulted above: */
        std::vector<Rational> large;
    };

    void swap(Expr& lhs, Expr& rhs);
    Expr operator"" _ex(const char* str, std::size_t);
    Expr operator"" _ex(unsigned long long n);
}
