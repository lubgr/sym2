#pragma once

#include "view.h"

namespace sym2 {
    struct Real;
    struct Complex;

    struct Number;
    struct Int;
    struct Rational;
    struct Double;

    struct Small; /* Everything that fits into 1 Blob. */
    struct Large; /* Everything > 1 Blob. */

    struct Scalar; /* A leaf in any expression tree. */
    struct Symbol;
    struct Constant;
    struct Sum;
    struct Product;
    struct Power;
    struct Function;

    struct NumericallyEvaluable;
    struct Positive;
    struct Negative;

    template <class FirstTag, class... RestTag>
    bool is(ExprView e)
    {
        return is<FirstTag>(e) && is<RestTag...>(e);
    }

    template <class FirstTag, class... RestTag>
    bool isOneOf(ExprView e)
    {
        if constexpr (sizeof...(RestTag) == 0)
            return is<FirstTag>(e);
        else
            return is<FirstTag>(e) || isOneOf<RestTag...>(e);
    }

    template <>
    bool is<Real>(ExprView e);
    template <>
    bool is<Complex>(ExprView e);
    template <>
    bool is<Number>(ExprView e);
    template <>
    bool is<Int>(ExprView e);
    template <>
    bool is<Rational>(ExprView e);
    template <>
    bool is<Double>(ExprView e);
    template <>
    bool is<Small>(ExprView e);
    template <>
    bool is<Large>(ExprView e);
    template <>
    bool is<Scalar>(ExprView e);
    template <>
    bool is<Symbol>(ExprView e);
    template <>
    bool is<Constant>(ExprView e);
    template <>
    bool is<Sum>(ExprView e);
    template <>
    bool is<Product>(ExprView e);
    template <>
    bool is<Power>(ExprView e);
    template <>
    bool is<Function>(ExprView e);
    template <>
    bool is<NumericallyEvaluable>(ExprView e);
    template <>
    bool is<Positive>(ExprView e);
    template <>
    bool is<Negative>(ExprView e);
}
