
#include <algorithm>
#include <array>
#include <vector>
#include "autosimpl.h"
#include "doctest/doctest.h"
#include "orderrelationimpl.h"

using namespace sym2;

TEST_CASE("Order relation")
{
    SUBCASE("Excessive ordered operands")
    {
        std::pmr::memory_resource* mr = std::pmr::get_default_resource();
        std::pmr::vector<Expr> symbols{mr};

        for (char c1 = 'a'; c1 <= 'z'; ++c1)
            for (char c2 = 'a'; c2 <= 'z'; ++c2) {
                const std::array<char, 2> data{{c1, c2}};
                const std::string_view name{data.data(), data.size()};
                symbols.emplace_back(name);
            }

        std::vector<ExprView<>> views;
        std::copy(symbols.cbegin(), symbols.cend(), std::back_inserter(views));

        const Expr lhs{CompositeType::sum, views, mr};
        const Expr rhs{CompositeType::sum, views, mr};

        CHECK_FALSE(productsOrSums(lhs, rhs));
        CHECK_FALSE(productsOrSums(rhs, lhs));
    }
}
