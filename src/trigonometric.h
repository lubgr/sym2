#pragma once

#include "sym2/expr.h"

namespace sym2 {
    Expr autoSin(ExprView<> arg, Expr::allocator_type allocator);
    Expr autoCos(ExprView<> arg, Expr::allocator_type allocator);
    Expr autoTan(ExprView<> arg, Expr::allocator_type allocator);
    Expr autoAsin(ExprView<> arg, Expr::allocator_type allocator);
    Expr autoAcos(ExprView<> arg, Expr::allocator_type allocator);
    Expr autoAtan(ExprView<> arg, Expr::allocator_type allocator);
    Expr autoAtan2(ExprView<> x2, ExprView<> x1, Expr::allocator_type allocator);

    // Wrappers around std:: counterparts. Since we use pointers to these functions, we need the
    // wrappers as it is not permitted to use references to standard library functions.
    double sin(double arg);
    double cos(double arg);
    double tan(double arg);
    double asin(double arg);
    double acos(double arg);
    double atan(double arg);
    double atan2(double x2, double x1);
}
