#pragma once

#include <array>
#include <span>
#include "expr.h"

namespace sym2 {
    Expr autoSum(std::span<ExprView> ops);
    Expr autoProduct(std::span<ExprView> ops);
    Expr autoPower(std::span<ExprView, 2> ops);
    Expr autoCpx(std::span<ExprView, 2> ops);

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

    template <class... T>
    Expr power(const T&... args)
    {
        return detail::construct(autoPower, args...);
    }

    template <class T>
    Expr oneOver(const T& arg)
    {
        static const Expr minusOne{-1};

        return power(arg, minusOne);
    }

    template <class... T>
    Expr cpx(const T&... args)
    {
        return detail::construct(autoCpx, args...);
    }
}
