#pragma once

#include <memory_resource>
#include <span>
#include <vector>
#include "expr.h"
#include "operandsview.h"
#include "predicates.h"
#include "sym2/functionview.h"

namespace sym2 {
    class ProductSimpl {
      public:
        struct Dependencies {
            FunctionView<bool(ExprView<>, ExprView<>)> orderLessThan;
            FunctionView<Expr(ExprView<>, ExprView<>)> autoPower;
            FunctionView<Expr(ExprView<>, ExprView<>)> autoSum;
            FunctionView<Expr(ExprView<number>, ExprView<number>)> numericMultiply;
        };

        ProductSimpl(Dependencies callbacks, std::pmr::memory_resource* buffer);

        Expr autoSimplify(std::span<const ExprView<>> ops);

      private:
        std::pmr::vector<Expr> intermediateSimplify(std::span<const ExprView<>> ops);
        std::pmr::vector<Expr> simplTwoFactors(ExprView<> lhs, ExprView<> rhs);
        std::pmr::vector<Expr> binaryProduct(ExprView<!product>, ExprView<!product>);

        std::pmr::vector<Expr> simplMoreThanTwoFactors(std::span<const ExprView<>> ops);
        template <class View>
        std::pmr::vector<Expr> merge(OperandsView p, View q);
        template <class View>
        std::pmr::vector<Expr> mergeNonEmpty(OperandsView p, View q);
        std::pmr::vector<Expr> prepend(ExprView<> first, std::pmr::vector<Expr>&& rest);

        Dependencies callbacks;
        std::pmr::memory_resource* buffer;
    };

}
