
#include "get.h"
#include "eval.h"

template <>
std::int32_t sym2::get<std::int32_t>(ExprView e)
{
    assert(isSmallInt(e));

    return e[0].main.exact.num;
}

template <>
sym2::SmallRational sym2::get<sym2::SmallRational>(ExprView e)
{
    assert(isSmallRational(e) || isSmallInt(e));

    return e[0].main.exact;
}

template <>
double sym2::get<double>(ExprView e)
{
    assert(isNumericallyEvaluable(e));

    return evalReal(e, [](auto&&...) {
        assert(false);
        return 0.0;
    });
}

template <>
sym2::LargeInt sym2::get<sym2::LargeInt>(ExprView e)
{
    assert(isInteger(e));

    if (isSmallInt(e))
        return LargeInt{get<std::int32_t>(e)};

    assert(isLargeInt(e));

    LargeInt result;

    const auto* first = reinterpret_cast<const unsigned char*>(std::next(e.data()));
    const auto* last = std::next(first, nChildBlobs(e) * sizeof(Blob));

    import_bits(result, first, last);

    return e[0].mid.largeIntSign * result;
}

template <>
sym2::LargeRational sym2::get<sym2::LargeRational>(ExprView e)
{
    assert(isRational(e) || isInteger(e));

    if (isInteger(e))
        return LargeRational{get<LargeInt>(e)};
    else if (isSmallRational(e)) {
        const auto value = get<SmallRational>(e);
        return LargeRational{value.num, value.denom};
    } else
        return LargeRational{get<LargeInt>(first(e)), get<LargeInt>(second(e))};
}

template <>
std::string_view sym2::get<std::string_view>(ExprView e)
{
    assert(isSymbolOrConstant(e) || isFunction(e));

    const Blob& blobWithName = isSymbolOrConstant(e) ? e[0] : e[1];

    return std::string_view{static_cast<const char*>(blobWithName.pre.name)};
}

template <>
sym2::UnaryDoubleFctPtr sym2::get<sym2::UnaryDoubleFctPtr>(ExprView e)
{
    assert(isFunction(e));

    return e[1].main.unaryEval;
}

template <>
sym2::BinaryDoubleFctPtr sym2::get<sym2::BinaryDoubleFctPtr>(ExprView e)
{
    assert(isFunction(e));

    return e[1].main.binaryEval;
}
