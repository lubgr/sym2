#pragma once

#include <memory_resource>
#include <span>
#include <vector>
#include "expr.h"
#include "operandsview.h"

namespace sym2 {
    std::pmr::vector<Expr> autoProductIntermediate(std::span<const ExprView<>> ops);
    std::pmr::vector<Expr> simplTwoFactors(ExprView<> lhs, ExprView<> rhs);
    std::pmr::vector<Expr> simplMoreThanTwoFactors(std::span<const ExprView<>> ops);
    template <class View>
    std::pmr::vector<Expr> merge(OperandsView p, View q);
    template <class View>
    std::pmr::vector<Expr> mergeNonEmpty(OperandsView p, View q);
}
