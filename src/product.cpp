
#include "product.h"
#include "query.h"

sym2::ProductExprVec sym2::autoProductIntermediate(SmallVecBase<ExprView>& ops)
{
    if (ops.size() == 2)
        return simplTwoFactors(ops.front(), ops.back());
    else
        return simplNFactors(ops);
}

sym2::ProductExprVec sym2::simplTwoFactors(ExprView lhs, ExprView rhs)
{
    (void) lhs;
    (void) rhs;

    return {};
}

sym2::ProductExprVec sym2::simplNFactors(SmallVecBase<ExprView>& ops)
{
    (void) ops;

    return {};
}
