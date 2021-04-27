#pragma once

#include "expr.h"
#include "predicates.h"
#include "smallvec.h"

struct s7_scheme;
using S7Ptr = struct s7_cell*;

namespace sym2 {
    class S7Converter {
      public:
        S7Converter(s7_scheme& sc);

        S7Ptr fromExpr(ExprView<> expression);
        S7Ptr fromExpr(ExprView<!scalar> expression);
        S7Ptr fromExpr(ExprView<scalar> expression);

        Expr listToSingleExpr(S7Ptr list);
        SmallVec<Expr, 10> listToExprVec(S7Ptr list);
        Expr toExpr(S7Ptr expression);

      private:
        s7_scheme& sc;
    };
}
