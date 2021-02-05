
#include <boost/range/algorithm.hpp>
#include "canonical.h"
#include "doctest/doctest.h"
#include "testutils.h"
#include "trigonometric.h"
#include "view.h"

using namespace sym2;

TEST_CASE("Semantic traversal")
{
    const LargeInt largeInt{"2323498273984729837498234029380492839489234902384"};
    const Expr li{largeInt};
    const auto p1 = product(2_ex, sum("a"_ex, "b"_ex));
    const auto p2 = product("c"_ex, "d"_ex, "e"_ex, "f"_ex);
    const auto fct = sym2::atan2("a"_ex, "b"_ex);
    const auto s = sum(li, p1, p2, fct);
    auto op = OperandIterator{s};

    SUBCASE("Nth child on increment")
    {
        CHECK(*op == view(li));
        CHECK(*++op == view(p1));
        CHECK(*++op == view(p2));
        CHECK(*++op == view(fct));
        CHECK(++op == OperandIterator{});
    }
}

TEST_CASE("Basic ExprView behavior")
{
    const auto s = sum("a"_ex, "b"_ex);
    const auto sv = view(s);

    SUBCASE("Size")
    {
        CHECK(sv.size() == 3);
    }

    SUBCASE("Copy and compare")
    {
        std::vector<Blob> dest;

        boost::copy(sv, std::back_inserter(dest));

        boost::equal(
          sv, dest, [](const Blob& lhs, const Blob& rhs) { return std::memcmp(&lhs, &rhs, sizeof(Blob)) == 0; });
    }
}
