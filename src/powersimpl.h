#pragma once

#include "expr.h"
#include "predicates.h"

namespace sym2 {
    Expr autoPowerImpl(ExprView<> base, ExprView<> exp);

    /* The exponent must not be zero: */
    Expr autoPowerImpl(ExprView<number && realDomain> base, std::int32_t exp);
    Expr powerRealBase(ExprView<number && realDomain> base, std::uint32_t exp);
}
