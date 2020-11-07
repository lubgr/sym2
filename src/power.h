#pragma once

#include <span>
#include "expr.h"

namespace sym2 {
    Expr power(std::span<ExprView, 2> ops);
}
