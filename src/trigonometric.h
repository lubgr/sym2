#pragma once

#include "expr.h"

namespace sym2 {
    Expr sin(ExprView arg);
    Expr cos(ExprView arg);
    Expr tan(ExprView arg);
    Expr asin(ExprView arg);
    Expr acos(ExprView arg);
    Expr atan(ExprView arg);
    Expr atan2(ExprView x2, ExprView x1);
}
