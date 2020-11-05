
#include "number.h"
#include <boost/functional/hash.hpp>
#include <limits>

sym2::Number::Number(int value)
    : Number(value, 1)
{}

sym2::Number::Number(int numerator, int denominator)
    : Number(Int(numerator), Int(denominator))
{}

sym2::Number::Number(double value)
    : rep(value)
{}

sym2::Number::Number(const Int& value)
    : Number(value, 1)
{}

sym2::Number::Number(const Int& numerator, const Int& denominator)
    /* The implementation doesn't move from input rvalues, hence const references are fine here: */
    : rep(std::in_place_type_t<Rational>(), denominator < 0 ? -numerator : numerator, abs(denominator))
{}

sym2::Number& sym2::Number::operator+=(const Number& rhs)
{
    rep = std::visit(Operate<std::plus<>>{}, rep, rhs.rep);

    return *this;
}

sym2::Number& sym2::Number::operator-=(const Number& rhs)
{
    return operator+=(-rhs);
}

sym2::Number& sym2::Number::operator*=(const Number& rhs)
{
    rep = std::visit(Operate<std::multiplies<>>{}, rep, rhs.rep);

    return *this;
}

sym2::Number& sym2::Number::operator/=(const Number& rhs)
{
    rep = std::visit(Operate<std::divides<>>{}, rep, rhs.rep);

    return *this;
}

const sym2::Number& sym2::Number::operator+() const
{
    return *this;
}

sym2::Number sym2::Number::operator-() const
{
    if (isRational())
        return Number{-numerator(), denominator()};
    else
        return Number{-toDouble()};
}

bool sym2::Number::isRational() const
{
    return std::holds_alternative<Rational>(rep);
}

bool sym2::Number::isDouble() const
{
    return std::holds_alternative<double>(rep);
}

sym2::Int sym2::Number::numerator() const
{
    if (isRational())
        return std::get<Rational>(rep).numerator();
    else
        return 0;
}

sym2::Int sym2::Number::denominator() const
{
    if (isRational())
        return std::get<Rational>(rep).denominator();
    else
        return 1;
}

double sym2::Number::toDouble() const
{
    if (isRational())
        return boost::rational_cast<double>(std::get<Rational>(rep));
    else
        return std::get<double>(rep);
}

bool sym2::operator==(const Number& lhs, const Number& rhs)
{
    if (lhs.isRational() && rhs.isRational())
        return lhs.numerator() == rhs.numerator() && lhs.denominator() == rhs.denominator();
    else
        return lhs.toDouble() == rhs.toDouble();
}

bool sym2::operator<(const Number& lhs, const Number& rhs)
{
    return lhs.toDouble() < rhs.toDouble();
}

std::size_t std::hash<sym2::Number>::operator()(const sym2::Number& n) const
{
    std::size_t seed = 0;

    boost::hash_combine(seed, n.toDouble());
    boost::hash_combine(seed, n.denominator());
    boost::hash_combine(seed, n.numerator());

    return seed;
}
