#pragma once

#include <span>
#include <vector>
#include "sym2/operandsview.h"
#include "sym2/expr.h"
#include "sym2/functionview.h"
#include "sym2/predicates.h"

namespace sym2 {
    class CohenAutoSimpl {
      public:
        struct Dependencies {
            FunctionView<bool(ExprView<>, ExprView<>)> orderLessThan;
            FunctionView<Expr(ExprView<number>, ExprView<number>)> numericAdd;
            FunctionView<Expr(ExprView<number>, ExprView<number>)> numericMultiply;
        };

        CohenAutoSimpl(Dependencies callbacks, Expr::allocator_type allocator);

        Expr simplifySum(std::span<const ExprView<>> ops);
        Expr simplifyProduct(std::span<const ExprView<>> ops);
        Expr simplifyPower(ExprView<> base, ExprView<> exp);

      private:
        Expr simplifySum(ExprView<> lhs, ExprView<> rhs);
        ScopedLocalVec<Expr> simplSumIntermediate(std::span<const ExprView<>> ops);
        ScopedLocalVec<Expr> simplTwoSummands(ExprView<> lhs, ExprView<> rhs);
        ScopedLocalVec<Expr> binarySum(ExprView<!sum>, ExprView<!sum>);

        ScopedLocalVec<Expr> simplMoreThanTwoSummands(std::span<const ExprView<>> ops);
        template <class View, class BinarySimplMember>
        ScopedLocalVec<Expr> merge(OperandsView p, View q, BinarySimplMember reduce);
        template <class View, class BinarySimplMember>
        ScopedLocalVec<Expr> mergeNonEmpty(OperandsView p, View q, BinarySimplMember reduce);
        ScopedLocalVec<Expr> prepend(ExprView<> first, ScopedLocalVec<Expr>&& rest);

        Expr simplifyProduct(ExprView<> lhs, ExprView<> rhs);
        Expr simplifyProduct(ExprView<> first, OperandsView rest);
        ScopedLocalVec<Expr> simplProductIntermediate(std::span<const ExprView<>> ops);
        ScopedLocalVec<Expr> simplTwoFactors(ExprView<> lhs, ExprView<> rhs);
        ScopedLocalVec<Expr> binaryProduct(ExprView<!product> lhs, ExprView<!product> rhs);
        ScopedLocalVec<Expr> simplMoreThanTwoFactors(std::span<const ExprView<>> ops);

        // The exponent must not be zero:
        Expr computePowerRationalToInt(ExprView<rational> base, std::int16_t exp);
        Expr computePowerRationalToUnsigned(ExprView<rational> base, std::uint16_t exp);
        Expr simplPowerRationalToRational(
          ExprView<rational> base, ExprView<rational && !integer> exp);

        Dependencies callbacks;
        Expr::allocator_type allocator;
    };
}
