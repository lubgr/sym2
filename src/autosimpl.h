#pragma once

#include <array>
#include <span>
#include "expr.h"

namespace sym2 {
    Expr autoSum(std::span<ExprView> ops);
    Expr autoProduct(std::span<ExprView> ops);
    Expr autoPower(ExprView b, ExprView exp);
    Expr autoCpx(ExprView real, ExprView imag);

    namespace detail {
        template <class... T, class Simplifier>
        Expr construct(Simplifier f, const T&... args)
        {
            std::array<ExprView, sizeof...(T)> views{args...};

            return std::invoke(f, views);
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

    inline Expr power(ExprView base, ExprView exp)
    {
        return autoPower(base, exp);
    }

    inline Expr oneOver(ExprView arg)
    {
        static const Expr minusOne{-1};

        return power(arg, minusOne);
    }

    inline Expr cpx(ExprView real, ExprView imag)
    {
        return autoCpx(real, imag);
    }
}
