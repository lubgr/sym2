#pragma once

#include "binaryfct.h"
#include "expr.h"
#include "typetags.h"

namespace sym2 {
    Expr autoPowerImpl(ExprView b, ExprView exp, const BinaryOps& cb = {});

    /* The exponent must not be zero: */
    Expr autoPowerImpl(Tagged<Real, Number> b, std::int32_t exp, const BinaryOps& cb);
    Expr powerRealBase(Tagged<Real, Number> b, std::uint32_t exp, BinaryFct multiply);
}
