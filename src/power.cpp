
#include "power.h"
#include "get.h"
#include "query.h"
#include "typetags.h"

sym2::Expr sym2::autoPowerImpl(ExprView<> base, ExprView<> exp, const BinaryOps& cb)
{
    if (base == 0_ex || base == 1_ex)
        return Expr{base};
    else if (exp == 0_ex)
        return 1_ex;
    else if (exp == 1_ex)
        return Expr{base};
    else if (is<Real, Number>(base) && is<Small, Int>(exp))
        return autoPowerImpl(tag<Real, Number>(base), get<std::int32_t>(exp), cb);
    else if (is<Power>(base) && is<Int>(exp)) {
        const auto [_, origExp] = asPower(base);
        return cb.power(base, cb.product(exp, origExp));
    }

    return Expr{Type::power, {base, exp}};
}

sym2::Expr sym2::autoPowerImpl(Tagged<Real, Number> base, std::int32_t exp, const BinaryOps& cb)
{
    assert(exp != 0);

    const auto positiveExp = static_cast<std::uint32_t>(exp);
    const auto forPositiveExp = powerRealBase(base, positiveExp, cb.product);

    if (exp < 0)
        return cb.power(forPositiveExp, Expr{-1});

    return forPositiveExp;
}

sym2::Expr sym2::powerRealBase(Tagged<Real, Number> base, std::uint32_t exp, BinaryFct multiply)
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
