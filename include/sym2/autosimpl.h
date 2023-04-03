#pragma once

#include <initializer_list>
#include <span>
#include "expr.h"
#include "exprview.h"

namespace sym2 {
    Expr autoSum(ExprView<> lhs, ExprView<> rhs, Expr::allocator_type allocator);
    Expr autoSum(std::span<const ExprView<>> ops, Expr::allocator_type allocator);
    Expr autoSum(std::initializer_list<ExprView<>> ops, Expr::allocator_type allocator);

    Expr autoProduct(ExprView<> lhs, ExprView<> rhs, Expr::allocator_type allocator);
    Expr autoProduct(std::span<const ExprView<>> ops, Expr::allocator_type allocator);
    Expr autoProduct(std::initializer_list<ExprView<>> ops, Expr::allocator_type allocator);
    Expr autoMinus(ExprView<> arg, Expr::allocator_type allocator);

    Expr autoPower(ExprView<> base, ExprView<> exp, Expr::allocator_type allocator);
    Expr autoOneOver(ExprView<> arg, Expr::allocator_type allocator);

    Expr autoComplex(ExprView<> real, ExprView<> imag, Expr::allocator_type allocator);
}
