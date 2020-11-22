#pragma once

#include <cstdint>
#include <string_view>
#include "expr.h"

namespace sym2 {
    Type type(ExprView e);
    Sign sign(ExprView e);
    Flag flags(ExprView e);

    bool isScalar(ExprView e);

    bool isNumber(ExprView e);
    bool isInteger(ExprView e);
    bool isSymbolOrConstant(ExprView e);
    bool isSymbol(ExprView e);
    bool isConstant(ExprView e);
    bool isSum(ExprView e);
    bool isProduct(ExprView e);
    bool isPower(ExprView e);
    bool isFunction(ExprView e);
    bool isFunction(ExprView e, std::string_view name);

    bool isNumericallyEvaluable(ExprView e);

    ExprView base(ExprView e);
    ExprView exponent(ExprView e);

    bool isPositive(ExprView e);
    bool isNegative(ExprView e);

    std::size_t nOps(ExprView e);
}
