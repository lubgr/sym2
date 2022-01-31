#pragma once

#include <algorithm>
#include <string>
#include "expr.h"

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
}
