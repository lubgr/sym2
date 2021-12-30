#pragma once

#include <memory_resource>
#include "expr.h"
#include "largerational.h"
#include "predicates.h"
#include "view.h"

namespace sym2 {
    struct SmallRational;

    class NumberArithmetic {
      public:
        // The memory resource is currently unused. This might change in the future though, so we keep the parameter for
        // now.
        explicit NumberArithmetic(std::pmr::memory_resource* buffer);

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

        std::pmr::memory_resource* buffer;
    };
}
