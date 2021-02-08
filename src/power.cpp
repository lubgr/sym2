
#include "power.h"
#include "get.h"
#include "typetags.h"

sym2::Expr sym2::autoPowerImpl(ExprView b, ExprView exp, const BinaryOps& cb)
{
    if (b == 0_ex || b == 1_ex)
        return Expr{b};
    else if (exp == 0_ex)
        return 1_ex;
    else if (exp == 1_ex)
        return Expr{b};
    else if (is<Real, Number>(b) && is<Small, Int>(exp))
        return powerRealBase(b, get<std::int32_t>(exp), cb);
    else if (is<Power>(b) && is<Int>(exp))
        return cb.power(base(b), cb.product(exp, exponent(b)));

    return Expr{Type::power, {b, exp}};
}

sym2::Expr sym2::powerRealBase(ExprView b, std::int32_t exp, const BinaryOps& cb)
{
    assert((is<Real, Number>(b)));
    assert(exp != 0);

    const auto positiveExp = static_cast<std::uint32_t>(exp);
    const auto forPositiveExp = powerRealBase(b, positiveExp, cb.product);

    if (exp < 0)
        return cb.power(forPositiveExp, Expr{-1});

    return forPositiveExp;
}

sym2::Expr sym2::powerRealBase(ExprView base, std::uint32_t exp, BinaryFct multiply)
{
    /* Copied and adjusted from https://stackoverflow.com/questions/101439. */
    Expr increasingBase{base};
    Expr result = 1;

    while (true) {
        if (exp & 1)
            result = multiply(result, increasingBase);

        exp >>= 1;

        if (!exp)
            break;

        increasingBase = multiply(increasingBase, increasingBase);
    }

    return result;
}
