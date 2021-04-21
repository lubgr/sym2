#pragma once

#include <algorithm>
#include "expr.h"
#include "smallvec.h"

namespace sym2 {
    Expr autoSum(SmallVecBase<ExprView<>>&& ops);
    Expr autoProduct(SmallVecBase<ExprView<>>&& ops);
    Expr autoPower(ExprView<> base, ExprView<> exp);
    Expr autoCpx(ExprView<> real, ExprView<> imag);

    namespace detail {
        template <class... T, class Simplifier>
        Expr construct(Simplifier f, const T&... args)
        {
            /* The buffer might be increased within the simplification process. */
            constexpr std::size_t minBufferSize = 5;
            constexpr auto bufferSize = std::max<std::size_t>(minBufferSize, 2 * sizeof...(T));

            return std::invoke(f, SmallVec<ExprView<>, bufferSize>{args...});
        }
    }

    template <class... T>
    Expr sum(const T&... args)
    {
        return detail::construct(autoSum, args...);
    }

    template <class... T>
    Expr product(const T&... args)
    {
        return detail::construct(autoProduct, args...);
    }

    template <class... T>
    Expr minus(const T&... args)
    {
        static const Expr minusOne{-1};

        return product(minusOne, args...);
    }

    inline Expr power(ExprView<> base, ExprView<> exp)
    {
        return autoPower(base, exp);
    }

    inline Expr oneOver(ExprView<> arg)
    {
        static const Expr minusOne{-1};

        return power(arg, minusOne);
    }

    inline Expr cpx(ExprView<> real, ExprView<> imag)
    {
        return autoCpx(real, imag);
    }
}
