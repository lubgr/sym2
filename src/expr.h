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
        symbol = 1, /* Not starting at 0 helps pretty-printing in a debugger */
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
        unaryFunction,
        binaryFunction
    };

    enum class Flag : std::uint8_t { none, numericallyEvaluable };

    Flag operator|(Flag lhs, Flag rhs);
    Flag& operator|=(Flag& lhs, Flag rhs);
    Flag operator&(Flag lhs, Flag rhs);
    Flag& operator&=(Flag& lhs, Flag rhs);

    struct SmallRational {
        std::int32_t num;
        std::int32_t denom;
    };

    using UnaryDoubleFctPtr = double (*)(double);
    using BinaryDoubleFctPtr = double (*)(double, double);

    struct Operand {
        Type header;
        Flag flags;

        union Data6 {
            char name[6];
            std::int8_t largeIntSign;
        } pre;

        union Data8 {
            char name[8];
            std::size_t count;
            SmallRational exact;
            double inexact;
            UnaryDoubleFctPtr unaryEval;
            BinaryDoubleFctPtr binaryEval;
        } main;
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
        void appendSmallOrLargeInt(const Int& n);
        void appendLargeInt(const Int& n);
        void copyToFirstOrThrow(std::string_view name, std::uint8_t maxLength);

        SmallVec<Operand, 10> buffer;
    };

    Expr operator"" _ex(const char* str, std::size_t);
    Expr operator"" _ex(unsigned long long n);

    template <class T>
    T get(ExprView e) = delete;
    /* If there is a mismatch between requested and actual type, this results in UB. Still, these specializations try to
     * be as applicable as possible: anything that can be numerically evaluated can be retrieved as get<double>, and a
     * small integer can be accessed via get<Int> etc. */
    template <>
    std::int32_t get<std::int32_t>(ExprView e);
    template <>
    SmallRational get<SmallRational>(ExprView e);
    template <>
    Int get<Int>(ExprView e);
    template <>
    Rational get<Rational>(ExprView e);
    template <>
    std::string_view get<std::string_view>(ExprView e);
}
