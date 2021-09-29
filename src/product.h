#pragma once

#include <span>
#include "expr.h"
#include "operandsview.h"
#include "smallvec.h"

namespace sym2 {
    inline constexpr std::size_t productResultBufferSize = 10;
    using ProductExprVec = SmallVec<Expr, productResultBufferSize>;

    ProductExprVec autoProductIntermediate(std::span<const ExprView<>> ops);
    ProductExprVec simplTwoFactors(ExprView<> lhs, ExprView<> rhs);
    ProductExprVec simplNFactors(std::span<const ExprView<>> ops);
    ProductExprVec merge(OperandsView p, OperandsView q);
    ProductExprVec merge(ExprView<> p1, ExprView<> q1, OperandsView p, OperandsView q);
}
