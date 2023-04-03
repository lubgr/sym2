
#include "childiterator.h"
#include "doctest/doctest.h"
#include "sym2/expr.h"
#include "sym2/get.h"
#include "testutils.h"

using namespace sym2;

TEST_CASE("ChildIterator")
{
    const Expr::allocator_type alloc{};
    const LargeInt largeInt{"2323498273984729837498234029380492839489234902384"};
    const Expr li{largeInt, alloc};
    const Expr p1 = directProduct({2_ex, directSum({"a"_ex, "b"_ex}, alloc)}, alloc);
    const Expr p2 = directProduct({"c"_ex, "d"_ex, "e"_ex, "f"_ex}, alloc);
    const Expr fct{"atan2", "a"_ex, "b"_ex, std::atan2, alloc};
    const Expr s = directSum({li, fct, p1, p2}, alloc);

    SUBCASE("Large rational number")
    {
        const LargeRational lr{1, largeInt};
        const Expr n{lr, alloc};

        const auto first = ChildIterator::logicalChildren(n);
        const auto last = ChildIterator::logicalChildrenSentinel(n);

        // A large rational number doesn't have any logical operands
        CHECK(first == last);
    }

    SUBCASE("Nth child on increment")
    {
        auto op = ChildIterator::logicalChildren(s);

        CHECK(*op == li);
        CHECK(*++op == fct);
        CHECK(*++op == p1);
        CHECK(*++op == p2);
        CHECK(++op == ChildIterator::logicalChildrenSentinel(s));
    }

    SUBCASE("Single, artificial operand")
    {
        const Expr a{"a"_ex, alloc};
        auto op = ChildIterator::singleChild(a);

        CHECK(*op == *ChildIterator::logicalChildren(fct));
        CHECK(++op == ChildIterator::singleChildSentinel(a));
    }
}
