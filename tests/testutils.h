#pragma once

#include <algorithm>
#include <initializer_list>
#include <string>
#include "sym2/expr.h"

#define CHECK_RANGES_EQ(lhs, rhs)                                                                  \
    {                                                                                              \
        const auto& rng1 = lhs;                                                                    \
        const auto& rng2 = rhs;                                                                    \
        using std::cbegin;                                                                         \
        using std::cend;                                                                           \
        CHECK(std::equal(cbegin(rng1), cend(rng1), cbegin(rng2), cend(rng2)));                     \
    }

namespace sym2 {
    inline auto directComposite(CompositeType type, std::initializer_list<ExprView<>> operands,
      Expr::allocator_type allocator)
    {
        assert(operands.size() >= 2);

        return Expr{type, operands, allocator};
    }

    inline auto directProduct(
      std::initializer_list<ExprView<>> operands, Expr::allocator_type allocator)
    {
        return directComposite(CompositeType::product, operands, allocator);
    }

    inline auto directSum(
      std::initializer_list<ExprView<>> operands, Expr::allocator_type allocator)
    {
        return directComposite(CompositeType::sum, operands, allocator);
    }

    inline auto directPower(ExprView<> base, ExprView<> exp, Expr::allocator_type allocator)
    {
        return directComposite(CompositeType::power, {base, exp}, allocator);
    }

    inline auto directComplex(ExprView<> real, ExprView<> imag, Expr::allocator_type allocator)
    {
        return directComposite(CompositeType::complexNumber, {real, imag}, allocator);
    }
}
