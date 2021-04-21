
#include "trigonometric.h"
#include <cmath>

sym2::Expr sym2::sin(ExprView<> arg)
{
    return Expr{"sin", arg, std::sin};
}

sym2::Expr sym2::cos(ExprView<> arg)
{
    return Expr{"cos", arg, std::cos};
}

sym2::Expr sym2::tan(ExprView<> arg)
{
    return Expr{"tan", arg, std::tan};
}

sym2::Expr sym2::asin(ExprView<> arg)
{
    return Expr{"asin", arg, std::asin};
}

sym2::Expr sym2::acos(ExprView<> arg)
{
    return Expr{"acos", arg, std::acos};
}

sym2::Expr sym2::atan(ExprView<> arg)
{
    return Expr{"atan", arg, std::atan};
}

sym2::Expr sym2::atan2(ExprView<> x2, ExprView<> x1)
{
    return Expr{"atan2", x2, x1, std::atan2};
}
