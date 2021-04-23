#pragma once

#include "autosimpl.h"
#include "expr.h"
#include "functionview.h"

namespace sym2 {
    using BinaryFct = FunctionView<Expr(ExprView<>, ExprView<>)>;

    struct BinaryOps {
        BinaryFct product = sym2::autoProduct<ExprView<>, ExprView<>>;
        BinaryFct sum = sym2::autoSum<ExprView<>, ExprView<>>;
        BinaryFct power = sym2::autoPower;
    };
}
