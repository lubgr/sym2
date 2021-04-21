#pragma once

#include "expr.h"

namespace sym2 {
    Expr multiplyNumbers(ExprView<> lhs, ExprView<> rhs);
    Expr addNumbers(ExprView<> lhs, ExprView<> rhs);
}
