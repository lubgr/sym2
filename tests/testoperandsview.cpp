
#include <boost/range/algorithm.hpp>
#include "doctest/doctest.h"
#include "sym2/operandsview.h"
#include "sym2/autosimpl.h"
#include "sym2/expr.h"
#include "testutils.h"
#include "trigonometric.h"

using namespace sym2;

TEST_CASE("Semantic traversal")
{
    const Expr::allocator_type alloc{};
    const LargeInt largeInt{"2323498273984729837498234029380492839489234902384"};
    const Expr li{largeInt, alloc};
    const Expr fp{0.123456789, alloc};
    const Expr p1 = directProduct({2_ex, directSum({"a"_ex, "b"_ex}, alloc)}, alloc);
    const Expr p2 = directProduct({"c"_ex, "d"_ex, "e"_ex, "f"_ex}, alloc);
    const Expr fct{"atan2", "a"_ex, "b"_ex, std::atan2, alloc};
    const Expr s = directSum({li, fct, p1, p2}, alloc);

    SUBCASE("OperandsView")
    {
        const std::vector<ExprView<>> expected{li, fct, p1, p2};
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

    SUBCASE("Empty OperandsView")
    {
        CHECK(OperandsView{}.empty());
        CHECK(OperandsView{}.size() == 0);

        for ([[maybe_unused]] ExprView<> e : OperandsView{})
            CHECK(false);
    }

    SUBCASE("OperandsView of scalar leaves")
    {
        CHECK(OperandsView::operandsOf(42_ex).empty());
        CHECK(OperandsView::operandsOf("a"_ex).empty());
        CHECK(OperandsView::operandsOf(li).empty());
        CHECK(OperandsView::operandsOf(fp).empty());
    }

    SUBCASE("OperandsView of function")
    {
        const OperandsView ops = OperandsView::operandsOf(fct);

        CHECK(ops.size() == 2);
        CHECK(*ops.begin() == "a"_ex);
        CHECK(*std::next(ops.begin()) == "b"_ex);
    }

    SUBCASE("Single, artificial OperandsView")
    {
        const std::vector<ExprView<>> expected{p2};
        std::vector<ExprView<>> actual;

        boost::copy(OperandsView::singleOperand(p2), std::back_inserter(actual));

        CHECK(actual.size() == expected.size());
        CHECK_RANGES_EQ(actual, expected);
    }

    SUBCASE("OperandsView subview")
    {
        const OperandsView orig = OperandsView::operandsOf(s);
        std::vector<ExprView<>> actual;

        boost::copy(orig.subview(0), std::back_inserter(actual));
        CHECK_RANGES_EQ(actual, (std::vector<ExprView<>>{li, fct, p1, p2}));

        actual.clear();
        boost::copy(orig.subview(1), std::back_inserter(actual));
        CHECK_RANGES_EQ(actual, (std::vector<ExprView<>>{fct, p1, p2}));

        actual.clear();
        boost::copy(orig.subview(2, 1), std::back_inserter(actual));
        CHECK_RANGES_EQ(actual, (std::vector<ExprView<>>{p1}));

        actual.clear();
        boost::copy(orig.subview(0, 4), std::back_inserter(actual));
        CHECK_RANGES_EQ(actual, (std::vector<ExprView<>>{li, fct, p1, p2}));

        actual.clear();
        boost::copy(orig.subview(3), std::back_inserter(actual));
        CHECK_RANGES_EQ(actual, (std::vector<ExprView<>>{p2}));

        actual.clear();
        boost::copy(orig.subview(4), std::back_inserter(actual));
        CHECK(actual.empty());
    }

    SUBCASE("Equality comparison")
    {
        CHECK(OperandsView::operandsOf(s) == OperandsView::operandsOf(s));
        CHECK(OperandsView::operandsOf(s) != OperandsView::operandsOf(s).subview(0, 3));
    }
}
