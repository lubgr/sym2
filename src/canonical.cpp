
#include "canonical.h"
#include "power.h"
#include "product.h"
#include "sum.h"

sym2::Expr sym2::autoSum(std::span<const ExprView> ops)
{
    // TODO
    return {Tag::sum, ops};
}

sym2::Expr sym2::autoProduct(std::span<const ExprView> ops)
{
    // TODO
    return {Tag::product, ops};
}

sym2::Expr sym2::autoPower(std::span<const ExprView, 2> ops)
{
    return power(ops);
}
