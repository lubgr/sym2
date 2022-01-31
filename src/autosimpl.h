#pragma once

#include <array>
#include "expr.h"
#include "view.h"

namespace sym2 {
    Expr autoSum(ExprView<> lhs, ExprView<> rhs);
    Expr autoSum(std::span<const ExprView<>> ops);
    template <class... T>
    Expr autoSum(ExprView<> first, ExprView<> second, ExprView<> third, const T&... args)
    {
        return autoSum(std::array<ExprView<>, 3 + sizeof...(T)>{{first, second, third, args...}});
    }

    Expr autoProduct(ExprView<> lhs, ExprView<> rhs);
    Expr autoProduct(std::span<const ExprView<>> ops);
    template <class... T>
    Expr autoProduct(ExprView<> first, ExprView<> second, ExprView<> third, const T&... args)
    {
        return autoProduct(
          std::array<ExprView<>, 3 + sizeof...(T)>{{first, second, third, args...}});
    }
    Expr autoMinus(ExprView<> arg);

    Expr autoPower(ExprView<> base, ExprView<> exp);
    Expr autoOneOver(ExprView<> arg);

    Expr autoComplex(ExprView<> real, ExprView<> imag);
}
