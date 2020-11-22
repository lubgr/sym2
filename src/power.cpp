
#include "power.h"
#include "query.h"

sym2::Expr sym2::power(std::span<ExprView, 2> ops)
{
    const ExprView b = ops[0];
    const ExprView exp = ops[1];

    if (b == 0_ex || b == 1_ex)
        return Expr{b};
    else if (exp == 0_ex)
        return 1_ex;
    else if (exp == 1_ex)
        return Expr{b};

    return Expr{Type::power, ops};
}
