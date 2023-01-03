
#include "trigonometric.h"
#include <cmath>

sym2::Expr sym2::autoSin(ExprView<> arg, Expr::allocator_type allocator)
{
    return Expr{"sin", arg, &sym2::sin, allocator};
}

sym2::Expr sym2::autoCos(ExprView<> arg, Expr::allocator_type allocator)
{
    return Expr{"cos", arg, &sym2::cos, allocator};
}

sym2::Expr sym2::autoTan(ExprView<> arg, Expr::allocator_type allocator)
{
    return Expr{"tan", arg, &sym2::tan, allocator};
}

sym2::Expr sym2::autoAsin(ExprView<> arg, Expr::allocator_type allocator)
{
    return Expr{"asin", arg, &sym2::asin, allocator};
}

sym2::Expr sym2::autoAcos(ExprView<> arg, Expr::allocator_type allocator)
{
    return Expr{"acos", arg, &sym2::acos, allocator};
}

sym2::Expr sym2::autoAtan(ExprView<> arg, Expr::allocator_type allocator)
{
    return Expr{"atan", arg, &sym2::atan, allocator};
}

sym2::Expr sym2::autoAtan2(ExprView<> x2, ExprView<> x1, Expr::allocator_type allocator)
{
    return Expr{"atan2", x2, x1, &sym2::atan2, allocator};
}

double sym2::sin(const double arg)
{
    return std::sin(arg);
}

double sym2::cos(const double arg)
{
    return std::cos(arg);
}

double sym2::tan(const double arg)
{
    return std::tan(arg);
}

double sym2::asin(const double arg)
{
    return std::asin(arg);
}

double sym2::acos(const double arg)
{
    return std::acos(arg);
}

double sym2::atan(const double arg)
{
    return std::atan(arg);
}

double sym2::atan2(const double x2, const double x1)
{
    return std::atan2(x2, x1);
}
