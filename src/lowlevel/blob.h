#pragma once

#include <cstddef>
#include <cstdint>
#include <string_view>
#include "doublefctptr.h"
#include "smallrational.h"

namespace sym2 {
    enum class Type : std::uint8_t {
        symbol = 1, /* Not starting at 0 helps pretty-printing in a debugger */
        constant,
        smallInt,
        smallRational,
        largeInt,
        /* A large rational can still have one small/inplace integer (either numerator or
           denominator): */
        largeRational,
        floatingPoint,
        complexNumber,
        sum,
        product,
        power,
        function,
        functionId
    };

    enum class Flag : std::uint8_t {
        none = 0b0,
        numericallyEvaluable = 0b1,
        positive = 0b10,
        real = 0b100
    };

    Flag operator|(Flag lhs, Flag rhs);
    Flag& operator|=(Flag& lhs, Flag rhs);
    Flag operator&(Flag lhs, Flag rhs);
    Flag& operator&=(Flag& lhs, Flag rhs);

    struct Blob {
        Type header;
        Flag flags;

        union Data2 {
            char name[2];
        } pre;

        union Data4 {
            char name[4];
            std::int32_t largeIntSign; // Could be smaller, but same size is less error prone.
            std::uint32_t nLogicalOrPhysicalChildren;
        } mid;

        union Data8 {
            char name[8];
            std::size_t nChildBlobs;
            SmallRational exact;
            double inexact;
            UnaryDoubleFctPtr unaryEval;
            BinaryDoubleFctPtr binaryEval;
        } main;
    };

    // These are used from both Expr and ExprLiteral. All other manual Blob initialisation is done
    // from within the Expr constructors.
    Blob smallIntBlob(std::int32_t n);
    Blob floatingPointBlob(double n);
    Blob symbolBlob(std::string_view symbol);
}
