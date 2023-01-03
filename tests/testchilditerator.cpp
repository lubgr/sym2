
#include "childiterator.h"
#include "doctest/doctest.h"
#include "sym2/expr.h"
#include "sym2/get.h"
#include "testutils.h"

using namespace sym2;

TEST_CASE("ChildIterator")
{
    std::pmr::memory_resource* mr = std::pmr::get_default_resource();
    const LargeInt largeInt{"2323498273984729837498234029380492839489234902384"};
    const Expr li{largeInt, mr};
    const Expr p1 = directProduct(mr, 2_ex, directSum(mr, "a"_ex, "b"_ex));
    const Expr p2 = directProduct(mr, "c"_ex, "d"_ex, "e"_ex, "f"_ex);
    const Expr fct{"atan2", "a"_ex, "b"_ex, std::atan2, mr};
    const Expr s = directSum(mr, li, fct, p1, p2);

    SUBCASE("Large rational number")
    {
        const LargeRational lr{1, largeInt};
        const Expr n{lr, mr};

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
        const Expr a{"a"_ex, mr};
        auto op = ChildIterator::singleChild(a);

        CHECK(*op == *ChildIterator::logicalChildren(fct));
        CHECK(++op == ChildIterator::singleChildSentinel(a));
    }
}
