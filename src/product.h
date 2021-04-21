#pragma once

#include <span>
#include "expr.h"
#include "smallvec.h"

namespace sym2 {
    inline constexpr std::size_t productResultBufferSize = 10;
    using ProductExprVec = SmallVec<Expr, productResultBufferSize>;

    ProductExprVec autoProductIntermediate(SmallVecBase<ExprView<>>& ops);
    ProductExprVec simplTwoFactors(ExprView<> lhs, ExprView<> rhs);
    ProductExprVec simplNFactors(SmallVecBase<ExprView<>>& ops);
}
