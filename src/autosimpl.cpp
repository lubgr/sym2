
#include "autosimpl.h"
#include "get.h"
#include "power.h"
#include "product.h"
#include "sum.h"

sym2::Expr sym2::autoSumOf(SmallVecBase<ExprView<>>&& ops)
{
    // TODO
    return {Type::sum, ops};
}

sym2::Expr sym2::autoProductOf(SmallVecBase<ExprView<>>&& ops)
{
    // TODO
    return Expr{Type::product, ops};

    const auto res = autoProductIntermediate(ops);

    if (res.empty())
        return 1_ex;
    else if (res.size() == 1)
        return res.front();
    else
        return {Type::product, ops};
}

sym2::Expr sym2::autoPower(ExprView<> base, ExprView<> exp)
{
    return autoPowerImpl(base, exp);
}

sym2::Expr sym2::autoComplex(ExprView<> real, ExprView<> imag)
{
    // TODO
    return Expr{Type::complexNumber, {real, imag}};
}
