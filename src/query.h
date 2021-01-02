#pragma once

#include <cstdint>
#include <string_view>
#include "blob.h"

namespace sym2 {
    class ExprView;

    Type type(ExprView e);
    Flag flags(ExprView e);

    bool isScalar(ExprView e);

    bool isNumber(ExprView e);
    bool isRealNumber(ExprView e);
    bool isInteger(ExprView e);
    bool isSmallInt(ExprView e);
    bool isLargeInt(ExprView e);
    bool isRational(ExprView e);
    bool isSmallRational(ExprView e);
    bool isLargeRational(ExprView e);
    bool isDouble(ExprView e);
    bool isComplexNumber(ExprView e);

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

    std::size_t nLogicalOperands(ExprView e);
    std::size_t nLogicalOperands(Blob b);
    std::size_t nChildBlobs(ExprView e);
    std::size_t nChildBlobs(Blob b);

    /* Access to the sub-parts of types with > 1 Blob, like composites, large integers etc. Parameter n must be
     * positive. UB if there is no corresponding subpart, e.g. nth(smallIntExpr, 100). Note that this is not random
     * access, but worst case linear time complexity. */
    ExprView nth(ExprView e, std::uint32_t n);
    ExprView first(ExprView e);
    ExprView second(ExprView e);
}
