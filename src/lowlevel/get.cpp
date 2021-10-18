
#include "get.h"
#include <cassert>
#include "childiterator.h"
#include "eval.h"
#include "blob.h"
#include "predicates.h"

template <>
std::int32_t sym2::get<std::int32_t>(ExprView<> e)
{
    assert((is < small && integer > (e)));

    return e[0].main.exact.num;
}

template <>
sym2::SmallRational sym2::get<sym2::SmallRational>(ExprView<> e)
{
    assert((is<small>(e) && is<rational>(e)));

    return e[0].main.exact;
}

template <>
double sym2::get<double>(ExprView<> e)
{
    assert((is<numericallyEvaluable>(e)));

    if (is < floatingPoint || constant > (e))
        return e[0].main.inexact;
    else if (is < small && (integer || rational) > (e))
        return static_cast<double>(e[0].main.exact.num) / e[0].main.exact.denom;

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
        return LargeInt{get<std::int32_t>(e)};

    assert((is < large && integer > (e)));

    LargeInt result;

    const auto* first = reinterpret_cast<const unsigned char*>(std::next(e.data()));
    const auto* last = std::next(first, nPhysicalChildren(e) * sizeof(Blob));

    import_bits(result, first, last);

    return e[0].mid.largeIntSign * result;
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
    } else
        return LargeRational{get<LargeInt>(numerator(e)), get<LargeInt>(denominator(e))};
}

template <>
std::string_view sym2::get<std::string_view>(ExprView<> e)
{
    assert((is < symbol || constant || function > (e)));

    const Blob& blobWithName = is < symbol || constant > (e) ? e[0] : e[1];

    return std::string_view{static_cast<const char*>(blobWithName.pre.name)};
}

template <>
sym2::UnaryDoubleFctPtr sym2::get<sym2::UnaryDoubleFctPtr>(ExprView<> e)
{
    assert((is<function>(e)));

    return e[1].main.unaryEval;
}

template <>
sym2::BinaryDoubleFctPtr sym2::get<sym2::BinaryDoubleFctPtr>(ExprView<> e)
{
    assert((is<function>(e)));

    return e[1].main.binaryEval;
}
