#pragma once

#include "sym2/expr.h"

namespace sym2 {
    Expr log(ExprView<> arg, Expr::allocator_type alloc);
}
