#pragma once

#include "binaryfct.h"
#include "expr.h"
#include "predicates.h"

namespace sym2 {
    Expr autoPowerImpl(ExprView<> base, ExprView<> exp, const BinaryOps& cb = {});

    /* The exponent must not be zero: */
    Expr autoPowerImpl(ExprView<number && realDomain> base, std::int32_t exp, const BinaryOps& cb);
    Expr powerRealBase(ExprView<number && realDomain> base, std::uint32_t exp, BinaryFct multiply);
}
