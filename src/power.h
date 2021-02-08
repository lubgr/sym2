#pragma once

#include "binaryfct.h"
#include "expr.h"

namespace sym2 {
    Expr autoPowerImpl(ExprView b, ExprView exp, const BinaryOps& cb = {});

    /* Expects the base to be a real domain number: */
    Expr powerRealBase(ExprView b, std::int32_t exp, const BinaryOps& cb);
    Expr powerRealBase(ExprView base, std::uint32_t exp, BinaryFct multiply);
}
