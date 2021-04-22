#pragma once

#include <algorithm>
#include "expr.h"
#include "smallvec.h"

namespace sym2 {
    Expr autoSumOf(SmallVecBase<ExprView<>>&& ops);
    Expr autoProductOf(SmallVecBase<ExprView<>>&& ops);
    Expr autoPower(ExprView<> base, ExprView<> exp);
    Expr autoComplex(ExprView<> real, ExprView<> imag);

    namespace detail {
        template <class... T>
        auto toArgVec(const T&... args)
        {
            /* The buffer might be increased within the simplification process. */
            constexpr std::size_t minBufferSize = 5;
            constexpr auto bufferSize = std::max<std::size_t>(minBufferSize, 2 * sizeof...(T));

            return SmallVec<ExprView<>, bufferSize>{args...};
        }
    }

    template <class... T>
    Expr autoSum(const T&... args)
    {
        return autoSumOf(detail::toArgVec(args...));
    }

    template <class... T>
    Expr autoProduct(const T&... args)
    {
        return autoProductOf(detail::toArgVec(args...));
    }

    template <class... T>
    Expr autoMinus(const T&... args)
    {
        static const Expr minusOne{-1};

        return autoProduct(minusOne, args...);
    }

    inline Expr autoOneOver(ExprView<> arg)
    {
        static const Expr minusOne{-1};

        return autoPower(arg, minusOne);
    }
}
