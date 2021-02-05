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
    Expr sum(T&&... args)
    {
        return detail::construct(autoSum, std::forward<T>(args)...);
    }

    template <class... T>
    Expr product(T&&... args)
    {
        return detail::construct(autoProduct, std::forward<T>(args)...);
    }

    template <class... T>
    Expr power(T&&... args)
    {
        return detail::construct(autoPower, std::forward<T>(args)...);
    }

    template <class... T>
    Expr cpx(T&&... args)
    {
        return detail::construct(autoCpx, std::forward<T>(args)...);
    }
}
