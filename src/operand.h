#pragma once

#include <cstdint>

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
}
