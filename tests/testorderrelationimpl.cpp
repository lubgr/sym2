
#include <algorithm>
#include <array>
#include <vector>
#include "doctest/doctest.h"
#include "orderrelationimpl.h"
#include "sym2/autosimpl.h"

using namespace sym2;

TEST_CASE("Order relation")
{
    SUBCASE("Excessive ordered operands")
    {
        const Expr::allocator_type alloc{};
        ScopedLocalVec<Expr> symbols{alloc};

        for (char c1 = 'a'; c1 <= 'z'; ++c1)
            for (char c2 = 'a'; c2 <= 'z'; ++c2) {
                const std::array<char, 2> data{{c1, c2}};
                const std::string_view name{data.data(), data.size()};
                symbols.emplace_back(name);
            }

        const Expr lhs{CompositeType::sum, symbols, alloc};
        const Expr rhs{CompositeType::sum, symbols, alloc};

        CHECK_FALSE(productsOrSums(lhs, rhs));
        CHECK_FALSE(productsOrSums(rhs, lhs));
    }
}
