
#include "autosimpl.h"
#include "childiterator.h"
#include "doctest/doctest.h"
#include "testutils.h"
#include "trigonometric.h"

using namespace sym2;

TEST_CASE("ChildIterator")
{
    const LargeInt largeInt{"2323498273984729837498234029380492839489234902384"};
    const Expr li{largeInt};
    const Expr p1 = autoProduct(2_ex, autoSum("a"_ex, "b"_ex));
    const Expr p2 = autoProduct("c"_ex, "d"_ex, "e"_ex, "f"_ex);
    const Expr fct = sym2::atan2("a"_ex, "b"_ex);
    const Expr s = autoSum(li, p1, p2, fct);
    auto op = ChildIterator::logicalChildren(s);

    SUBCASE("Nth child on increment")
    {
        CHECK(*op == view(li));
        CHECK(*++op == view(p1));
        CHECK(*++op == view(p2));
        CHECK(*++op == view(fct));
        CHECK(++op == ChildIterator{});
    }

    SUBCASE("Single, artificial operand")
    {
        const Expr a = "a"_ex;
        auto op = ChildIterator::singleChild(a);

        CHECK(*op == *ChildIterator::logicalChildren(fct));
        CHECK(++op == ChildIterator{});
    }
}
