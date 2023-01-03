#pragma once

#include <memory_resource>
#include "sym2/expr.h"
#include "sym2/exprview.h"
#include "largerational.h"
#include "sym2/predicates.h"

namespace sym2 {
    struct SmallRational;

    class NumberArithmetic {
      public:
        // The memory resource is currently unused. This might change in the future though, so we
        // keep the parameter for now.
        explicit NumberArithmetic(std::pmr::memory_resource* mr);

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

        std::pmr::memory_resource* mr;
    };
}
