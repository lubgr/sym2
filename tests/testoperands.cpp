
#include <boost/range/algorithm.hpp>
#include "autosimpl.h"
#include "doctest/doctest.h"
#include "operands.h"
#include "testutils.h"
#include "trigonometric.h"

using namespace sym2;

TEST_CASE("Semantic traversal")
{
    const LargeInt largeInt{"2323498273984729837498234029380492839489234902384"};
    const Expr li{largeInt};
    const auto p1 = autoProduct(2_ex, autoSum("a"_ex, "b"_ex));
    const auto p2 = autoProduct("c"_ex, "d"_ex, "e"_ex, "f"_ex);
    const auto fct = sym2::atan2("a"_ex, "b"_ex);
    const auto s = autoSum(li, p1, p2, fct);
    auto op = OperandIterator::fromComposite(s);

    SUBCASE("Nth child on increment")
    {
        CHECK(*op == view(li));
        CHECK(*++op == view(p1));
        CHECK(*++op == view(p2));
        CHECK(*++op == view(fct));
        CHECK(++op == OperandIterator{});
    }

    SUBCASE("Single, artificial operand")
    {
        const Expr a = "a"_ex;
        auto op = OperandIterator::fromSingle(a);

        CHECK(*op == *OperandIterator::fromComposite(fct));
        CHECK(++op == OperandIterator{});
    }

    SUBCASE("OperandsView")
    {
        const std::vector<ExprView<>> expected{li, p1, p2, fct};
        std::vector<ExprView<>> actual;

        boost::copy(OperandsView::fromComposite(s), std::back_inserter(actual));

        CHECK_RANGES_EQ(actual, expected);
    }

    SUBCASE("OperandsView size")
    {
        CHECK(OperandsView::fromComposite(s).size() == 4);
        CHECK(OperandsView::fromComposite(p1).size() == 2);

        CHECK(OperandsView::fromSingle(li).size() == 1);
        CHECK(OperandsView::fromSingle(s).size() == 1);
    }

    SUBCASE("Single, artificial OperandsView")
    {
        const std::vector<ExprView<>> expected{s};
        std::vector<ExprView<>> actual;

        boost::copy(OperandsView::fromSingle(s), std::back_inserter(actual));

        CHECK_RANGES_EQ(actual, expected);
    }

    SUBCASE("OperandsView subview")
    {
        const OperandsView orig = OperandsView::fromComposite(s);
        std::vector<ExprView<>> actual;

        boost::copy(orig.subview(1), std::back_inserter(actual));
        CHECK_RANGES_EQ(actual, (std::vector<ExprView<>>{p1, p2, fct}));

        actual.clear();
        boost::copy(orig.subview(2, 1), std::back_inserter(actual));
        CHECK_RANGES_EQ(actual, (std::vector<ExprView<>>{p2}));

        actual.clear();
        boost::copy(orig.subview(0, 4), std::back_inserter(actual));
        CHECK_RANGES_EQ(actual, (std::vector<ExprView<>>{li, p1, p2, fct}));

        actual.clear();
        boost::copy(orig.subview(3), std::back_inserter(actual));
        CHECK_RANGES_EQ(actual, (std::vector<ExprView<>>{fct}));
    }
}
