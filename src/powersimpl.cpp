
#include "powersimpl.h"
#include "autosimpl.h"
#include "get.h"
#include "query.h"

sym2::Expr sym2::autoPowerImpl(ExprView<> base, ExprView<> exp)
{
    if (base == 0_ex || base == 1_ex)
        return Expr{base};
    else if (exp == 0_ex)
        return 1_ex;
    else if (exp == 1_ex)
        return Expr{base};
    else if (is < realDomain && number > (base) && is < integer && small > (exp))
        return autoPowerImpl(base, get<std::int32_t>(exp));
    else if (is<power>(base) && is<integer>(exp)) {
        const auto [_, origExp] = splitAsPower(base);
        return autoPowerImpl(base, autoProduct(exp, origExp));
    }

    return Expr{CompositeType::power, {base, exp}};
}

sym2::Expr sym2::autoPowerImpl(ExprView<number && realDomain> base, std::int32_t exp)
{
    assert(exp != 0);

    const auto positiveExp = static_cast<std::uint32_t>(exp);
    const auto forPositiveExp = powerRealBase(base, positiveExp);

    if (exp < 0)
        return autoPowerImpl(forPositiveExp, Expr{-1});

    return forPositiveExp;
}

sym2::Expr sym2::powerRealBase(ExprView<number && realDomain> base, std::uint32_t exp)
{
    /* Copied and adjusted from https://stackoverflow.com/questions/101439. */
    Expr increasingBase{base};
    Expr result = 1;

    while (true) {
        if (exp & 1)
            result = autoProduct(result, increasingBase);

        exp >>= 1;

        if (!exp)
            break;

        increasingBase = autoProduct(increasingBase, increasingBase);
    }

    return result;
}
