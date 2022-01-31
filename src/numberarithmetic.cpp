
#include "numberarithmetic.h"
#include <array>
#include <functional>
#include "get.h"
#include "query.h"
#include "smallrational.h"

sym2::NumberArithmetic::NumberArithmetic(std::pmr::memory_resource* buffer)
    : buffer{buffer}
{}

sym2::Expr sym2::NumberArithmetic::multiply(ExprView<number> lhs, ExprView<number> rhs)
{
    if (isOneOf<complexDomain>(lhs, rhs))
        return multiplyComplex(lhs, rhs);
    else if (areAll<rational>(lhs, rhs))
        return reduceViaLargeRational(std::multiplies<>{}, lhs, rhs);
    else
        return reduceViaFloatingPoint(std::multiplies<>{}, lhs, rhs);
}

sym2::Expr sym2::NumberArithmetic::multiplyComplex(ExprView<number> lhs, ExprView<number> rhs)
{
    // The operands might not be both complex numbers, >= one should be at this point.
    const Expr real1 = multiply(real(lhs), real(rhs));
    const Expr imag1 = multiply(real(lhs), imag(rhs));
    const Expr imag2 = multiply(imag(lhs), real(rhs));
    const Expr real2 = multiply(imag(lhs), imag(rhs));
    const std::array<Expr, 2> operands{{subtract(real1, real2), add(imag1, imag2)}};

    return Expr{CompositeType::complexNumber, operands, buffer};
}

template <class Operation>
sym2::Expr sym2::NumberArithmetic::reduceViaLargeRational(
  Operation op, ExprView<number> lhs, ExprView<number> rhs)
{
    const LargeRational result = op(get<LargeRational>(lhs), get<LargeRational>(rhs));

    return Expr{LargeRationalRef{result}, buffer};
}

template <class Operation>
sym2::Expr sym2::NumberArithmetic::reduceViaFloatingPoint(
  Operation op, ExprView<number> lhs, ExprView<number> rhs)
{
    const double result = op(get<double>(lhs), get<double>(rhs));

    return Expr{result, buffer};
}

sym2::Expr sym2::NumberArithmetic::add(ExprView<number> lhs, ExprView<number> rhs)
{
    if (isOneOf<complexDomain>(lhs, rhs))
        return addComplex(lhs, rhs);
    else if (areAll<rational>(lhs, rhs))
        return reduceViaLargeRational(std::plus<>{}, lhs, rhs);
    else
        return reduceViaFloatingPoint(std::plus<>{}, lhs, rhs);
}

sym2::Expr sym2::NumberArithmetic::addComplex(ExprView<number> lhs, ExprView<number> rhs)
{
    const std::array<Expr, 2> operands{{add(real(lhs), real(rhs)), add(imag(lhs), imag(rhs))}};

    return Expr{CompositeType::complexNumber, operands, buffer};
}

sym2::Expr sym2::NumberArithmetic::subtract(ExprView<number> lhs, ExprView<number> rhs)
{
    if (isOneOf<complexDomain>(lhs, rhs))
        return subtractComplex(lhs, rhs);
    else if (areAll<rational>(lhs, rhs))
        return reduceViaLargeRational(std::minus<>{}, lhs, rhs);
    else
        return reduceViaFloatingPoint(std::minus<>{}, lhs, rhs);
}

sym2::Expr sym2::NumberArithmetic::subtractComplex(ExprView<number> lhs, ExprView<number> rhs)
{
    const std::array<Expr, 2> operands{
      {subtract(real(lhs), real(rhs)), subtract(imag(lhs), imag(rhs))}};

    return Expr{CompositeType::complexNumber, operands, buffer};
}
