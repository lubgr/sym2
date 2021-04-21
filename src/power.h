#pragma once

#include "binaryfct.h"
#include "expr.h"
#include "typetags.h"

namespace sym2 {
    Expr autoPowerImpl(ExprView<> base, ExprView<> exp, const BinaryOps& cb = {});

    /* The exponent must not be zero: */
    Expr autoPowerImpl(ExprView<Real, Number> base, std::int32_t exp, const BinaryOps& cb);
    Expr powerRealBase(ExprView<Real, Number> base, std::uint32_t exp, BinaryFct multiply);
}
