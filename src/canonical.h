#pragma once

#include <array>
#include <span>
#include <tuple>
#include <type_traits>
#include "expr.h"

namespace sym2 {
    Expr autoSum(std::span<ExprView> ops);
    Expr autoProduct(std::span<ExprView> ops);
    Expr autoPower(std::span<ExprView, 2> ops);

    namespace detail {
        template <class T>
        decltype(auto) literalToExpr(T&& arg)
        {
            using Type = std::remove_cvref_t<T>;

            if constexpr (std::is_same_v<Type, ExprView>)
                return arg;
            else if constexpr (std::is_same_v<Type, Expr>)
                return std::forward<T>(arg);
            else
                return Expr{std::forward<T>(arg)};
        }

        template <class... T>
        auto literalsToExpr(T&&... args)
        {
            return std::make_tuple(literalToExpr(std::forward<T>(args))...);
        }

        template <class Tuple>
        constexpr auto tupleToExprViewArray(const Tuple& from)
        {
            constexpr auto toStdArray = [](const auto&... arg) {
                return std::array<ExprView, std::tuple_size_v<Tuple>>{(arg)...};
            };

            return std::apply(toStdArray, from);
        }

        template <class... T, class Simplifier>
        Expr construct(Simplifier f, T&&... args)
        {
            const auto withLifetimeWhenNecessary = literalsToExpr(std::forward<T>(args)...);
            std::array<ExprView, sizeof...(T)> views = tupleToExprViewArray(withLifetimeWhenNecessary);

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
}
