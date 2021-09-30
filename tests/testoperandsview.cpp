
#include <boost/range/algorithm.hpp>
#include "autosimpl.h"
#include "doctest/doctest.h"
#include "operandsview.h"
#include "testutils.h"
#include "trigonometric.h"

using namespace sym2;

TEST_CASE("Semantic traversal")
{
    const LargeInt largeInt{"2323498273984729837498234029380492839489234902384"};
    const Expr li{largeInt};
    const Expr p1 = autoProduct(2_ex, autoSum("a"_ex, "b"_ex));
    const Expr p2 = autoProduct("c"_ex, "d"_ex, "e"_ex, "f"_ex);
    const Expr fct = sym2::atan2("a"_ex, "b"_ex);
    const Expr s = autoSum(li, p1, p2, fct);

    SUBCASE("OperandsView")
    {
        const std::vector<ExprView<>> expected{li, p1, p2, fct};
        std::vector<ExprView<>> actual;

        boost::copy(OperandsView::operandsOf(s), std::back_inserter(actual));

        CHECK_RANGES_EQ(actual, expected);
    }

    SUBCASE("OperandsView size")
    {
        CHECK(OperandsView::operandsOf(s).size() == 4);
        CHECK(OperandsView::operandsOf(p1).size() == 2);

        CHECK(OperandsView::singleOperand(li).size() == 1);
        CHECK(OperandsView::singleOperand(s).size() == 1);
    }

    SUBCASE("Single, artificial OperandsView")
    {
        const std::vector<ExprView<>> expected{s};
        std::vector<ExprView<>> actual;

        boost::copy(OperandsView::singleOperand(s), std::back_inserter(actual));

        CHECK_RANGES_EQ(actual, expected);
    }

    SUBCASE("OperandsView subview")
    {
        const OperandsView orig = OperandsView::operandsOf(s);
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

        actual.clear();
        boost::copy(orig.subview(4), std::back_inserter(actual));
        CHECK(actual.empty());
    }
}
