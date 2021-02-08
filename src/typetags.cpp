
#include "typetags.h"
#include "query.h"

template <>
bool sym2::is<sym2::Real>(ExprView e)
{
    // TODO This is incomplete, we need a tag for symbols to be marked real
    switch (type(e)) {
        case Type::smallInt:
        case Type::smallRational:
        case Type::floatingPoint:
        case Type::largeInt:
        case Type::largeRational:
            return true;
        default:
            return false;
    }
}

template <>
bool sym2::is<sym2::Complex>(ExprView e)
{
    // TODO This is incomplete, we need a tag for symbols to be marked complex
    return type(e) == Type::complexNumber;
}

template <>
bool sym2::is<sym2::Number>(ExprView e)
{
    switch (type(e)) {
        case Type::smallInt:
        case Type::smallRational:
        case Type::floatingPoint:
        case Type::largeInt:
        case Type::largeRational:
        case Type::complexNumber:
            return true;
        default:
            return false;
    }
}
template <>
bool sym2::is<sym2::Int>(ExprView e)
{
    const Type t = type(e);

    return t == Type::smallInt || t == Type::largeInt;
}

template <>
bool sym2::is<sym2::Rational>(ExprView e)
{
    switch (type(e)) {
        case Type::smallInt:
        case Type::smallRational:
        case Type::largeInt:
        case Type::largeRational:
            return true;
        default:
            return false;
    }
}

template <>
bool sym2::is<sym2::Double>(ExprView e)
{
    return type(e) == Type::floatingPoint;
}

template <>
bool sym2::is<sym2::Small>(ExprView e)
{
    switch (type(e)) {
        case Type::smallInt:
        case Type::smallRational:
        case Type::symbol:
        case Type::floatingPoint:
        case Type::constant:
            return true;
        default:
            return false;
    }
}

template <>
bool sym2::is<sym2::Large>(ExprView e)
{
    return !is<Small>(e);
}

template <>
bool sym2::is<sym2::Scalar>(ExprView e)
{
    switch (type(e)) {
        case Type::sum:
        case Type::product:
        case Type::power:
        case Type::function:
            return false;
        default:
            return true;
    }
}

template <>
bool sym2::is<sym2::Symbol>(ExprView e)
{
    return type(e) == Type::symbol;
}

template <>
bool sym2::is<sym2::Constant>(ExprView e)
{
    return type(e) == Type::constant;
}

template <>
bool sym2::is<sym2::Sum>(ExprView e)
{
    return type(e) == Type::sum;
}

template <>
bool sym2::is<sym2::Product>(ExprView e)
{
    return type(e) == Type::product;
}

template <>
bool sym2::is<sym2::Power>(ExprView e)
{
    return type(e) == Type::power;
}

template <>
bool sym2::is<sym2::Function>(ExprView e)
{
    return type(e) == Type::function;
}

template <>
bool sym2::is<sym2::NumericallyEvaluable>(ExprView e)
{
    return (flags(e) & Flag::numericallyEvaluable) != Flag::none;
}
