#pragma once

#include "sym2/largerational.h"
#include "sym2/expr.h"
#include "sym2/exprview.h"
#include "sym2/predicates.h"

namespace sym2 {
    struct SmallRational;

    class NumberArithmetic {
      public:
        // The memory resource is used to construct return objects from member functions.
        explicit NumberArithmetic(Expr::allocator_type allocator);

        Expr multiply(ExprView<number> lhs, ExprView<number> rhs);
        Expr add(ExprView<number> lhs, ExprView<number> rhs);
        Expr subtract(ExprView<number> lhs, ExprView<number> rhs);

      private:
        Expr multiplyComplex(ExprView<number> lhs, ExprView<number> rhs);
        template <class Operation>
        Expr reduceViaLargeRational(Operation op, ExprView<number> lhs, ExprView<number> rhs);
        template <class Operation>
        Expr reduceViaFloatingPoint(Operation op, ExprView<number> lhs, ExprView<number> rhs);
        Expr addComplex(ExprView<number> lhs, ExprView<number> rhs);
        Expr subtractComplex(ExprView<number> lhs, ExprView<number> rhs);

        Expr::allocator_type allocator;
    };
}
