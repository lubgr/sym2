
#include "numberarithmetic.h"
#include "get.h"
#include "smallrational.h"

sym2::NumberArithmetic::NumberArithmetic(std::pmr::memory_resource* buffer)
    : buffer{buffer}
{}

sym2::Expr sym2::NumberArithmetic::multiply(ExprView<number> lhs, ExprView<number> rhs)
{
    if (areAll<rational>(lhs, rhs))
        return multiplyRationals(get<LargeRational>(lhs, buffer), get<LargeRational>(rhs, buffer));

    return 1_ex;
}

sym2::Expr sym2::NumberArithmetic::multiplyRationals(LargeRational&& lhs, LargeRational&& rhs)
{
    lhs *= std::move(rhs);

    return Expr{LargeRationalRef{lhs}};
}
