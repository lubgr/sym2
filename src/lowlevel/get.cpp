
#include "sym2/get.h"
#include <cassert>
#include "blobapi.h"
#include "sym2/eval.h"
#include "sym2/predicates.h"
#include "sym2/query.h"

template <>
std::int16_t sym2::get<std::int16_t>(ExprView<> e)
{
    assert((is < small && integer > (e)));

    return getSmallInt(*e.get());
}

template <>
sym2::SmallRational sym2::get<sym2::SmallRational>(ExprView<> e)
{
    assert(is < small && rational > (e));

    return getSmallRational(*e.get());
}

template <>
double sym2::get<double>(ExprView<> e)
{
    assert((is<numericallyEvaluable>(e)));

    if (is < floatingPoint || constant > (e)) {
        return getFloatingPoint(e.get());
    } else if (is < small && (integer || rational) > (e)) {
        const SmallRational rational = get<SmallRational>(e);

        return static_cast<double>(rational.num) / rational.denom;
    }

    return evalReal(e, [](auto&&...) {
        assert(false);
        return 0.0;
    });
}

template <>
sym2::LargeInt sym2::get<sym2::LargeInt>(ExprView<> e)
{
    assert((is<integer>(e)));

    if (is < small && integer > (e))
        return LargeInt{get<std::int16_t>(e)};

    assert((is < large && integer > (e)));

    return getLargeInt(e.get());
}

template <>
sym2::LargeRational sym2::get<sym2::LargeRational>(ExprView<> e)
{
    assert((is<rational>(e)));

    if (is<integer>(e))
        return LargeRational{get<LargeInt>(e)};
    else if (is < small && rational > (e)) {
        const auto value = get<SmallRational>(e);
        return LargeRational{value.num, value.denom};
    } else {
        const ExprView<integer> num{getNumeratorFromLargeRational(e.get())};
        const ExprView<integer> denom{getDenominatorFromLargeRational(e.get())};

        return LargeRational{get<LargeInt>(num), get<LargeInt>(denom)};
    }
}

template <>
std::string_view sym2::get<std::string_view>(ExprView<> e)
{
    assert((is < symbol || constant || function > (e)));

    if (is<symbol>(e))
        return getSymbolName(e.get());
    else if (is<constant>(e))
        return getConstantName(e.get());
    else
        return getFunctionName(e.get());
}

template <>
sym2::UnaryDoubleFctPtr sym2::get<sym2::UnaryDoubleFctPtr>(ExprView<> e)
{
    assert((is<function>(e)));

    return getUnaryDoubleFctPtr(e.get());
}

template <>
sym2::BinaryDoubleFctPtr sym2::get<sym2::BinaryDoubleFctPtr>(ExprView<> e)
{
    assert((is<function>(e)));

    return getBinaryDoubleFctPtr(e.get());
}
