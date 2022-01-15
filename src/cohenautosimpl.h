#pragma once

#include <memory_resource>
#include <span>
#include <vector>
#include "expr.h"
#include "operandsview.h"
#include "predicates.h"
#include "sym2/functionview.h"

namespace sym2 {
    class CohenAutoSimpl {
      public:
        struct Dependencies {
            FunctionView<bool(ExprView<>, ExprView<>)> orderLessThan;
            FunctionView<Expr(ExprView<number>, ExprView<number>)> numericAdd;
            FunctionView<Expr(ExprView<number>, ExprView<number>)> numericMultiply;
        };

        CohenAutoSimpl(Dependencies callbacks, std::pmr::memory_resource* buffer);

        Expr simplifySum(std::span<const ExprView<>> ops);
        Expr simplifyProduct(std::span<const ExprView<>> ops);
        Expr simplifyPower(ExprView<> base, ExprView<> exp);

      private:
        Expr simplifySum(ExprView<> lhs, ExprView<> rhs);
        std::pmr::vector<Expr> simplSumIntermediate(std::span<const ExprView<>> ops);
        std::pmr::vector<Expr> simplTwoSummands(ExprView<> lhs, ExprView<> rhs);
        std::pmr::vector<Expr> binarySum(ExprView<!sum>, ExprView<!sum>);

        std::pmr::vector<Expr> simplMoreThanTwoSummands(std::span<const ExprView<>> ops);
        template <class View, class BinarySimplMember>
        std::pmr::vector<Expr> merge(OperandsView p, View q, BinarySimplMember reduce);
        template <class View, class BinarySimplMember>
        std::pmr::vector<Expr> mergeNonEmpty(OperandsView p, View q, BinarySimplMember reduce);
        std::pmr::vector<Expr> prepend(ExprView<> first, std::pmr::vector<Expr>&& rest);

        Expr simplifyProduct(ExprView<> lhs, ExprView<> rhs);
        Expr simplifyProduct(ExprView<> first, OperandsView rest);
        std::pmr::vector<Expr> simplProductIntermediate(std::span<const ExprView<>> ops);
        std::pmr::vector<Expr> simplTwoFactors(ExprView<> lhs, ExprView<> rhs);
        std::pmr::vector<Expr> binaryProduct(ExprView<!product> lhs, ExprView<!product> rhs);
        std::pmr::vector<Expr> simplMoreThanTwoFactors(std::span<const ExprView<>> ops);

        Expr floatingPointPower(double base, double exp);
        // The exponent must not be zero:
        Expr simplPowerRealNumberBase(ExprView<number && realDomain> base, std::int32_t exp);
        Expr computePower(ExprView<number && realDomain> base, std::uint32_t exp);

        Dependencies callbacks;
        std::pmr::memory_resource* buffer;
    };
}
