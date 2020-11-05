
#include "power.h"
#include "query.h"

sym2::Expr sym2::power(std::span<const ExprView, 2> ops)
{
    const auto [b, exp] = ops;

    if (b == 0 || b == 1)
        return Expr{b};
    else if (exp == 0)
        return Expr{1};
    else if (exp == 1)
        return Expr{b};
    // TODO
    // else if (isNumber(b) && isInteger(exp))
    //     return integerExp(b, exp);
    // else if (isPower(b) && isInteger(exp))
    //     return power(base(b), product(exponent(b), exp));

    return Expr{Tag::power, ops};
}
