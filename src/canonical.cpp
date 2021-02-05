
#include "canonical.h"
#include "power.h"
#include "product.h"
#include "sum.h"

sym2::Expr sym2::autoSum(std::span<ExprView> ops)
{
    // TODO
    return {Type::sum, ops};
}

sym2::Expr sym2::autoProduct(std::span<ExprView> ops)
{
    // TODO
    return {Type::product, ops};
}

sym2::Expr sym2::autoPower(std::span<ExprView, 2> ops)
{
    return power(ops);
}

sym2::Expr sym2::autoCpx(std::span<ExprView, 2> ops)
{
    // TODO
    return Expr{Type::complexNumber, ops};
}
