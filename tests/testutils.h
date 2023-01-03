#pragma once

#include <algorithm>
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
    inline ExprView<> view(const Expr& e)
    {
        return e;
    }

    template <class... T>
    auto directComposite(CompositeType type, typename Expr::allocator_type allocator, T&&... args)
    {
        static_assert(sizeof...(args) >= 2);
        std::pmr::vector<Expr> operands{{Expr{args, allocator}...}, allocator};

        return Expr{type, std::move(operands), allocator};
    }

    template <class... T>
    auto directProduct(typename Expr::allocator_type allocator, T&&... args)
    {
        return directComposite(CompositeType::product, allocator, std::forward<T>(args)...);
    }

    template <class... T>
    auto directSum(typename Expr::allocator_type allocator, T&&... args)
    {
        return directComposite(CompositeType::sum, allocator, std::forward<T>(args)...);
    }

    template <class T, class S>
    auto directPower(typename Expr::allocator_type allocator, T&& base, S&& exponent)
    {
        return directComposite(
          CompositeType::power, allocator, std::forward<T>(base), std::forward<S>(exponent));
    }

    template <class T, class S>
    auto directComplex(typename Expr::allocator_type allocator, T&& real, S&& imag)
    {
        return directComposite(
          CompositeType::complexNumber, allocator, std::forward<T>(real), std::forward<S>(imag));
    }
}
