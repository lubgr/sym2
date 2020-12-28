
#include <boost/range/algorithm.hpp>
#include "canonical.h"
#include "doctest/doctest.h"
#include "testutils.h"
#include "view.h"

using namespace sym2;

TEST_CASE("Semantic traversal")
{
    const Int largeInt{"2323498273984729837498234029380492839489234902384"};
    const auto s = sum(largeInt, product(2, sum("a", "b")), product("c", "d"));
    auto op = ConstSemanticOpIterator{s};

    SUBCASE("Point to first operand after construction")
    {
        const Expr li{largeInt};
        const auto expected = view(li);

        CHECK(expected == *op);
    }
}

TEST_CASE("Basic ExprView behavior")
{
    const auto s = sum("a", "b");
    const auto sv = view(s);

    SUBCASE("Empty")
    {
        ExprView empty;

        CHECK(empty.size() == 0);
        CHECK(empty.empty());
    }

    SUBCASE("Size")
    {
        CHECK(sv.size() == 3);
    }

    SUBCASE("Copy and compare")
    {
        std::vector<Operand> dest;

        boost::copy(sv, std::back_inserter(dest));

        boost::equal(sv, dest,
          [](const Operand& lhs, const Operand& rhs) { return std::memcmp(&lhs, &rhs, sizeof(Operand)) == 0; });
    }
}
