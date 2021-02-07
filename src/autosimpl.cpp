
#include "autosimpl.h"
#include "get.h"
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
    return Expr{Type::product, ops};

    const auto res = autoProductIntermediate(ops);

    if (res.empty())
        return 1_ex;
    else if (res.size() == 1)
        return res.front();
    else
        return {Type::product, ops};
}

sym2::Expr sym2::autoPower(ExprView b, ExprView exp)
{
    if (b == 0_ex || b == 1_ex)
        return Expr{b};
    else if (exp == 0_ex)
        return 1_ex;
    else if (exp == 1_ex)
        return Expr{b};
    else if (isNumber(b) && isSmallInt(exp))
        return numberPower(b, get<std::int32_t>(exp), product<ExprView, ExprView>);
    else if (isPower(b) && isInteger(exp))
        return power(base(b), product(exp, exponent(b)));

    return Expr{Type::power, {b, exp}};
}

sym2::Expr sym2::autoCpx(ExprView real, ExprView imag)
{
    // TODO
    return Expr{Type::complexNumber, {real, imag}};
}
